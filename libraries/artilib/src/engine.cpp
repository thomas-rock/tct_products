//=============================================================================
// Treasure Coast Technologies, Inc.          www.TreasureCoastTechnologies.com
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from RockSolid Solutions.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2026 to the present, Treasure Coast Technologies, Inc.
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
//
// Description :
//
//=============================================================================
#include "engine.h"
#include "parser.h"
#include "message.h"

#include <QDir>
#include <QRandomGenerator>
#include <QRegularExpression>

using namespace artiLib;

Engine::Engine (const QStringList& incpaths) : QObject(), m_rtf(false), m_plaintext(true), m_markdown(false), m_errors(0), m_warnings(0)
{
   // make incpaths absolute paths
   foreach (QString path, incpaths) {
      QDir dir(path);
      QString cpath = dir.canonicalPath();
      if (!cpath.isEmpty()) m_incpaths.append(cpath);
   }

   m_doc = std::make_shared<Doc>();
   connect(m_doc.get(), SIGNAL(message(MessageType,QString,QString,int,int)), this, SLOT(handleMessage(MessageType, QString,QString,int,int)));
}
Engine::~Engine ()
{
   m_parseData.clear();
   while (m_variableStack.size()) delete m_variableStack.pop();
}
//----------------------------------------------------------------------------
void Engine::setRtf (bool state) {m_rtf = state;}
void Engine::setPlaintext (bool state) {m_plaintext = state;}
void Engine::setMarkdown (bool state) {m_markdown = state;}
void Engine::setMaxLoops (int max) {m_maxLoops = max;}
//----------------------------------------------------------------------------
bool Engine::check (const QString& filename)
{
   Parser p;
   connect(&p, SIGNAL(message(MessageType,QString,QString,int,int)), this, SLOT(handleMessage(MessageType, QString,QString,int,int)));
   p.parse(filename);
   return p.status();
}

// Note: 'arti' argument should be template name, not file name
bool Engine::render(const QString& arti, QVariantMap contexts)
{
   QString templateName;
   QString subtemplateName;
   QString artiPath;

   // arti may be (sub)template name or filename - handle both options
   // arti as filename
   QFileInfo afi(arti);
   if (afi.suffix() == ("arti")) {
      templateName = afi.baseName();
      artiPath = findFile(arti);
   }

   // arti as (sub)template name
   else {

      // check if sub-template
      if (arti.contains("::")) {
         QStringList fields = arti.split("::");
         if (fields.size() > 2) handleMessage(SysWarning, QString("Extraneous sub-template names of '%1' ignored").arg(arti));
         templateName = fields[0];
         subtemplateName = fields[1];
      }
      else {
         templateName = arti;
         subtemplateName = QString();
      }

      // locate fullpath of arti file
      artiPath = (templateName.isEmpty() && m_filenames.size()) ? m_filenames.last() : findFile(templateName + ".arti");
   }

   if (artiPath.isEmpty()) return false;
   if (!templateName.isEmpty()) m_filenames.push(artiPath);

   ArtiNodePtr artiNode = parse(artiPath);
   if (artiNode == nullptr) return false;

   // create namespace variables
   if (m_variables != nullptr) m_variableStack.push(m_variables);
   m_variables = new QVariantMap;

   // add contexts to variables
   foreach (QString name, contexts.keys())
      m_variables->insert(name, contexts.value(name));

   // get template/sub-template root node
    ArtiNodePtr rootNode = nullptr;
   if (subtemplateName.isEmpty()) rootNode = artiNode->children.at(0);
   else {
      for (int index = 1; index < artiNode->children.size(); index++)
         if (artiNode->children.at(index)->text == subtemplateName) {
            rootNode = artiNode->children.at(index);
            break;
         }
   }

   // execute template
   if (rootNode) {
      // first save off and initialize indent info
      m_indentStack.push(m_indent);
      m_indent = 0;
      foreach (ArtiNodePtr child, rootNode->children)
         exec(child);

      // restore indent
      int prevIndent = m_indentStack.pop();
      m_doc->setIndentDelta(prevIndent - (m_indent + prevIndent));
      m_indent = prevIndent;
   }

   // cleanup
   delete m_variables;
   if (m_variableStack.size()) m_variables = m_variableStack.pop();
   else m_variables = nullptr;
   if (!templateName.isEmpty()) m_filenames.pop();

   return true;
}
QString Engine::artifact () {return (m_rtf)       ? m_doc->toHtml()     :
                                    (m_plaintext) ? m_doc->toString()   :
                                    (m_markdown)  ? m_doc->toMarkdown() : m_doc->toString();
                            }
//-----------------------------------------------------------------------------
void Engine::handleMessage (MessageType type, const QString& msg, const QString& file, int line, int col)
{
   if (type == SysWarning || type == UserWarning) m_warnings++;
   else if (type == SysError || type == UserError) m_errors++;
   MESSAGE(type, msg, file, line, col);
}
//-----------------------------------------------------------------------------
 ArtiNodePtr Engine::parse(const QString& artiPath)
{
    ArtiNodePtr artiNode;
   if (m_parseData.contains(artiPath))
      m_currentParseData = m_parseData.value(artiPath);
   else {
      Parser p;
      connect(&p, SIGNAL(message(MessageType, QString, QString,int,int)), this, SLOT(handleMessage(MessageType, QString,QString,int,int)));
      m_currentParseData = p.parse(artiPath);
      if (!p.status()) return nullptr;
      m_parseData.insert(artiPath, m_currentParseData);
   }
   return m_currentParseData->root;
}
void Engine::exec ( ArtiNodePtr n)
{
//qDebug() << QString("Exec - token: %1, name: %2, value: %3, size: %4").arg(n->token).arg(artiNode::name(n->token)).arg(n->text()).arg(n->size());
   switch (n->token)
   {
      case ArtiToken::Align       :
      {
         if      (m_rtf) handleMessage(SysWarning, QString("align tag ignored in rtf mode"), m_filenames.last(), n->loc.line, n->loc.position);
         else if (m_markdown) handleMessage(SysWarning, QString("'align' tag ignored in markdown mode"), m_filenames.last(), n->loc.line, n->loc.position);
         else m_doc->align(eval(n->children.at(0)).toString());
         break;
      }
      case ArtiToken::Eval        : m_doc->addText(toString(eval(n->children.at(0))));break;
      case ArtiToken::For         :
      {
         m_loopCount++;
         LoopData* ld = new LoopData();
         m_loopStack.push(ld);
          ArtiNodePtr varNode   = n->children.at(0);
         QString varName     = varNode->text;
         int childCount      = n->children.size();

         // from,to specification
         if (childCount == 4)       // var, from, to, stmt
         {
            QVariant fromVar   = eval(n->children.at(1));
            QVariant toVar     = eval(n->children.at(2));
             ArtiNodePtr stmtNode = n->children.at(3);

            bool isChar = isString(fromVar) && isString(toVar) && fromVar.toString().length() == 1 && toVar.toString().length() == 1;
            int from    = (isChar) ? int(fromVar.toString().at(0).toLatin1()) : fromVar.toInt();
            int to      = (isChar) ? int(toVar.toString().at(0).toLatin1())   : toVar.toInt();

            bool inc_loop = (from <= to);
            int i = from;
            int loop = 0;
            while ((inc_loop && i <= to) || (!inc_loop && i >= to)) {
               if (isChar)
                  m_variables->insert(varName, QString(QChar(char(i))));
               else
                  m_variables->insert(varName, i);

               if (inc_loop) {
                  ld->set(loop, (to - i), (i == to), (i == from));
                  i++;
               }
               else {
                  ld->set(loop, (i - to), (i == to), (i == from));
                  i--;
               }

               foreach ( ArtiNodePtr elemNode, stmtNode->children)
                  exec(elemNode);

               loop++;
               if (loop > m_maxLoops) {
                  handleMessage(SysWarning, QString("Excessive for-loop iterations. May be infinite loop. Aborting for-loop"), m_filenames.last(), n->loc.line, n->loc.position);
                  break;
               }
            }
         }

         // object iterator - object must be an array or can be converted to an array
         //    var, list, stmt
         else
         {
            QVariant param = eval(n->children.at(1));

            QVariantList forArray;
            if (isArray(param))
               forArray = param.toList();
            else if (isMap(param))
               forArray = param.toMap().values();
            else
               forArray.append(param);

            int loop = 0;
            int count = forArray.size()-1;
             ArtiNodePtr stmtNode = n->children.at(2);
            foreach (QVariant value, forArray)
            {
               if (!value.isValid()) continue;  // probably empty array
               ld->set(loop, (count-loop), (loop == count), (loop == 0));
               m_variables->insert(varName, value);
               foreach ( ArtiNodePtr elemNode, stmtNode->children)
                  exec(elemNode);

               loop++;
               if (loop > m_maxLoops) {
                  handleMessage(SysWarning, QString("Excessive for-loop iterations. May be infinite loop. Aborting for-loop"), m_filenames.last(), n->loc.line, n->loc.position);
                  break;
               }
            }
         }

         if (m_loopCount) {
            m_loopCount--;
            delete m_loopStack.pop();
         }
         break;
      }
      case ArtiToken::If          :
      {
         int index = 0;
          ArtiNodePtr ifNode = n;
         while (index < n->children.size()) {
            if (index == 0) index = 2;
            else ifNode = n->children.at(index++);

            if (ifNode->token == ArtiToken::Else) {
               foreach ( ArtiNodePtr elemNode, ifNode->children)
                  exec(elemNode);
            }
            else {         // if or elsif
               if (eval(ifNode->children.at(0)).toBool()) {
                  foreach ( ArtiNodePtr elemNode, ifNode->children.at(1)->children)
                     exec(elemNode);
                  break;
               }
            }
         }
         break;
      }
      case ArtiToken::Include     :
      {
         QString artiName = eval(n->children.at(0)).toString();
         QVariantMap with;
         for (int index = 1; index < n->children.size(); index++) {
             ArtiNodePtr assignNode = n->children.at(index);
            with.insert(assignNode->children.at(0)->text, eval(assignNode->children.at(1)));
         }
         render(artiName, with);

         break;
      }
      case ArtiToken::Load        :
      {
         QString scriptName = eval(n->children.at(0)).toString();
         if (scriptName.isEmpty()) break;

         QString fn = findFile(scriptName);
         if (fn.isEmpty()) break;

         QFile sfile(fn);
         if (!sfile.open(QIODevice::ReadOnly)) break;
         QTextStream sstream(&sfile);
         QString scriptCode = sstream.readAll();
         QJSValue result = m_jsengine.evaluate(scriptCode, fn);
         if (result.isError()) handleMessage(SysError, QString("%1:%2 - Uncaught exception : %3").arg(result.property("fileName").toString()).arg(result.property("lineNumber").toInt()).arg(result.toString()));
         break;
      }
      case ArtiToken::Info     :
      {
         QString msg = eval(n->children.at(0)).toString();
         for (int child = 1; child < n->children.count(); child++)
            msg = msg.arg(eval(n->children.at(child)).toString());

         handleMessage(UserInfo, msg, m_filenames.last(), n->loc.line, n->loc.position);

         break;
      }
      case ArtiToken::Debug     :
      {
         QString msg = eval(n->children.at(0)).toString();
         for (int child = 1; child < n->children.count(); child++)
            msg = msg.arg(eval(n->children.at(child)).toString());

         handleMessage(UserDebug, msg, m_filenames.last(), n->loc.line, n->loc.position);

         break;
      }
      case ArtiToken::Status     :
      {
         QString msg = eval(n->children.at(0)).toString();
         for (int child = 1; child < n->children.count(); child++)
            msg = msg.arg(eval(n->children.at(child)).toString());

         handleMessage(UserStatus, msg, m_filenames.last(), n->loc.line, n->loc.position);

         break;
      }
      case ArtiToken::Warning     :
      {
         QString msg = eval(n->children.at(0)).toString();
         for (int child = 1; child < n->children.count(); child++)
            msg = msg.arg(eval(n->children.at(child)).toString());

         handleMessage(UserWarning, msg, m_filenames.last(), n->loc.line, n->loc.position);

         break;
      }
      case ArtiToken::Error     :
      {
         QString msg = eval(n->children.at(0)).toString();
         for (int child = 1; child < n->children.count(); child++)
            msg = msg.arg(eval(n->children.at(child)).toString());

         handleMessage(UserError, msg, m_filenames.last(),n->loc.line, n->loc.position);

         break;
      }
      case ArtiToken::Fatal     :
      {
         QString msg = eval(n->children.at(0)).toString();
         for (int child = 1; child < n->children.count(); child++)
            msg = msg.arg(eval(n->children.at(child)).toString());

         handleMessage(UserFatal, msg, m_filenames.last(),n->loc.line, n->loc.position);

         break;
      }
      case ArtiToken::Print     :
      {
         QString text = eval(n->children.at(0)).toString();
         for (int child = 1; child < n->children.count(); child++)
            text = text.arg(eval(n->children.at(child)).toString());

         writeText(text);

         break;
      }
      case ArtiToken::Set         :
      {
         foreach ( ArtiNodePtr assignNode, n->children)
         {
            QString name = assignNode->children.at(0)->text;
            if (name.startsWith('_')) m_globals.insert(name, eval(assignNode->children.at(1)));
            else m_variables->insert(name, eval(assignNode->children.at(1)));
         }
         break;
      }
      case ArtiToken::Template    : break;       // failsafe - not executed
      case ArtiToken::Text        : writeText(n->text); break;
      case ArtiToken::While       :
      {
         bool valid = eval(n->children.at(0)).toBool();
          ArtiNodePtr stmtNode = n->children.at(1);

         int loop_count = 0;
         while (valid) {
            foreach ( ArtiNodePtr elemNode, stmtNode->children)
               exec(elemNode);

            valid = eval(n->children.at(0)).toBool();
            loop_count++;
            if (loop_count > m_maxLoops) {
               handleMessage(SysWarning, QString("Excessive while-loop iterations. May be infinite loop. Aborting while-loop"), m_filenames.last(),n->loc.line, n->loc.position);
               valid = false;
            }
         }
         break;
      }
   }
}
QVariant Engine::eval ( ArtiNodePtr n)
{
//qDebug() << QString("Eval - token: %1, name: %2, value: %3, size: %4").arg(n->token).arg(artiNode::name(n->token)).arg(n->text()).arg(n->size());
   quint32 token = n->token;
   if (token == ArtiToken::Id) {
      QString name = n->text;
      QVariant value = (name.startsWith('_')) ? m_globals.value(name, QVariant()) : m_variables->value(name, QVariant());
      QVariant nextValue;
      foreach ( ArtiNodePtr child, n->children) {
         if (!value.isValid()) break;
         else {
            if (child->token == ArtiToken::Id) {
               if (isMap(value)) nextValue = value.toMap().value(child->text);
            }
            else if (child->token == ArtiToken::Integer) {
               if (isMap(value)) nextValue = value.toMap().value(child->text);
               else if (isArray(value)) {
                  int index = child->text.toInt();
                  QVariantList list = value.toList();
                  if (list.size() && index >= 0 && index < list.size())
                     nextValue = list[child->text.toInt()];
               }
            }
            else {         // expression
               QVariant cvalue = eval(child);
               if (isMap(value)) nextValue = value.toMap().value(cvalue.toString());
               else if (isArray(value)) {
                  int index = cvalue.toInt();
                  QVariantList list = value.toList();
                  if (list.size() && index >= 0 && index < list.size())
                     nextValue = list[index];
               }
            }
            value = nextValue;
         }
      }
      return value;
   }
   else if (token == ArtiToken::Sysid) {
      QString name = n->text;
      if (name == "$loop")
         return (m_loopCount) ? m_loopStack.top()->loop : 0;
      else if (name == "$looprem")
         return (m_loopCount) ? m_loopStack.top()->rem  : 0;
      else if (name == "$loopfirst")
         return (m_loopCount) ? m_loopStack.top()->first  : 0;
      else if (name == "$looplast")
         return (m_loopCount) ? m_loopStack.top()->last  : 0;
      else return m_variables->value(name, QVariant());
   }
   else if (token == ArtiToken::Array) {
      QVariantList array;
      foreach ( ArtiNodePtr exprNode, n->children)
         array.append(eval(exprNode));

      return array;
   }
   else if (token == ArtiToken::Map) {
      QVariantMap map;
      for (int index = 0; index < n->children.count(); index += 2)
         map.insert(eval(n->children.at(index)).toString(), eval(n->children.at(index+1)));

      return map;
   }
   else if (token == ArtiToken::Real) {
      qreal r = n->text.toDouble();
      return r;
   }
   else if (token == ArtiToken::True)    return true;
   else if (token == ArtiToken::False)   return false;
   else if (token == ArtiToken::Null)    return QVariant();
   else if (token == ArtiToken::Integer) return n->text.toInt();
   else if (token == ArtiToken::SString) return n->text;
   else if (token == ArtiToken::DString) {
      QString rtn = n->text;
      rtn.replace("\\n", "\n");
      return rtn;
   }
   else if (token == ArtiToken::Regex) return n->text;
   else if (token == ArtiToken::Func) {
      QString funcName = n->text;
      QVariantList args;
      foreach ( ArtiNodePtr arg, n->children)
         args.append(eval(arg));
      return doFunc(funcName, args,n->loc.line, n->loc.position);
   }
   else if (token == ArtiToken::EqTilde)      // regex
   {
      QString str = eval(n->children.at(0)).toString();
      QRegularExpression regex(eval(n->children.at(1)).toString());
      if (!regex.isValid()) return false;
      QRegularExpressionMatch match = regex.match(str);
      if (!match.hasMatch()) return false;
      QStringList captured = match.capturedTexts();
      for (int index = 0; index < captured.size(); index++)
         m_variables->insert(QString("$%1").arg(index), captured[index]);
      return true;
   }

   // unop
   if (n->children.count() == 1) {
      QVariant right = eval(n->children.at(0));
      bool rreal = isReal(right);
      switch (n->token)
      {
         case '!'                : return (right.toBool()) ? 0 : 1;
         case ArtiToken::Minus   : if (rreal) return 0.0 - right.toReal();
                                   else       return 0 - right.toInt();
         default                 : return QVariant();
      }
   }

   // binop
   QVariant left  = eval(n->children.at(0));
   QVariant right = eval(n->children.at(1));
   bool lstring   = isString(left);
   bool lreal     = isReal(left);
   bool lint      = isInt(left);
   bool lbool     = isBool(left);
   bool rstring   = isString(right);
   bool rreal     = isReal(right);
   bool rint      = isInt(right);
   bool rbool     = isBool(right);

   // check for compatible types
   if (lstring && rstring)
      return binop_string(n->token, left.toString(), right.toString());
   else if (lreal && rreal)
      return binop_real(n->token, left.toReal(), right.toReal());
   else if (lint && rint)
      return binop_int(n->token, left.toInt(), right.toInt());
   else if (lbool && rbool)
      return binop_bool(n->token, left.toBool(), right.toBool());

   // convert types - prioritize real, int, bool then string
   bool ok;
   if (lreal) {
      if (rint)  return binop_real(n->token, left.toReal(), right.toReal());
      if (rbool) return binop_real(n->token, left.toReal(), right.toReal());
      // rtype is string
      qreal r = right.toReal(&ok);
      if (ok) return binop_real(n->token, left.toReal(), r);
      return binop_string(n->token, left.toString(), right.toString());
   }
   if (lint) {
      if (rreal) return binop_real(n->token, left.toReal(), right.toReal());
      if (rbool) return binop_real(n->token, left.toReal(), right.toReal());
      // rtype is string
      qreal r = right.toInt(&ok);
      if (ok) return binop_int(n->token, left.toReal(), r);
      return binop_string(n->token, left.toString(), right.toString());
   }
   if (lbool) {
      if (rint) return binop_int(n->token, left.toInt(), right.toInt());
      if (rreal) return binop_real(n->token, left.toReal(), right.toReal());
      // rtype is string
      qreal r = right.toInt(&ok);
      if (ok) return binop_int(n->token, left.toReal(), r);
      return binop_string(n->token, left.toString(), right.toString());
   }
   // ltype is string
   if (rreal) {
      qreal l = left.toReal(&ok);
      if (ok) return binop_real(n->token, l, right.toReal());
      return binop_string(n->token, left.toString(), right.toString());
   }
   // rtype is int
   if (rint) {
      int l = left.toInt(&ok);
      if (ok) return binop_int(n->token, l, right.toInt());
      return binop_string(n->token, left.toString(), right.toString());
   }
   // rtype is bool
   if (rbool) {
      bool l = left.toBool();
      return binop_bool(n->token, l, right.toBool());
   }

   // otherwise....
   return binop_string(n->token, left.toString(), right.toString());
}

QVariant Engine::binop_string (quint32 op, const QString& l, const QString& r)
{
   switch (op)
   {
      case '+'             : return l + r;
      case '&'             : return (!l.isEmpty() && !r.isEmpty()) ? 1 : 0;
      case '|'             : return (!l.isEmpty() || !r.isEmpty()) ? 1 : 0;
      case '^'             : return (!l.isEmpty() ^  !r.isEmpty()) ? 1 : 0;
      case '<'             : return (l < r) ? 1 : 0;
      case '>'             : return (l > r) ? 1 : 0;
      case ArtiToken::Eq   : return (l == r) ? 1 : 0;
      case ArtiToken::Neq  : return (l != r) ? 1 : 0;
      case ArtiToken::Lte  : return (l <= r) ? 1 : 0;
      case ArtiToken::Gte  : return (l >= r) ? 1 : 0;
      default              : return QVariant();
   }
}

QVariant Engine::binop_real (quint32 op, const qreal& l, const qreal r)
{
   switch (op)
   {
      case '*'             : return l * r;
      case '/'             : return (r == 0.0) ? 0.0 : l / r;
      case '+'             : return l + r;
      case '-'             : return l - r;
      case '&'             : return (l != 0.0 && r != 0.0) ? 1 : 0;
      case '|'             : return (l != 0.0 || r != 0.0) ? 1 : 0;
      case '<'             : return (l <  r) ? 1 : 0;
      case '>'             : return (l >  r) ? 1 : 0;
      case ArtiToken::Eq   : return (l == r) ? 1 : 0;
      case ArtiToken::Neq  : return (1 != r) ? 1 : 0;
      case ArtiToken::Lte  : return (l <= r) ? 1 : 0;
      case ArtiToken::Gte  : return (l >= r) ? 1 : 0;
      default              : return QVariant();
   }
}

QVariant Engine::binop_int (quint32 op, const int& l, const int& r)
{
   switch (op)
   {
      case '*'             : return l * r;
      case '/'             : return (r == 0) ? 0 : l / r;
      case '+'             : return l + r;
      case '-'             : return l - r;
      case '&'             : return (l && r) ? 1 : 0;
      case '|'             : return (l || r) ? 1 : 0;
      case '<'             : return (l <  r) ? 1 : 0;
      case '>'             : return (l >  r) ? 1 : 0;
      case ArtiToken::Eq   : return (l == r) ? 1 : 0;
      case ArtiToken::Neq  : return (1 != r) ? 1 : 0;
      case ArtiToken::Lte  : return (l <= r) ? 1 : 0;
      case ArtiToken::Gte  : return (l >= r) ? 1 : 0;
      default              : return QVariant();
   }
}

QVariant Engine::binop_bool (quint32 op, const bool& l, const bool& r)
{
   switch (op)
   {
      case '*'             : return l * r;
      case '/'             : return (r) ? l : false;      // avoid divide by false (0)
      case '+'             : return l + r;
      case '-'             : return l - r;
      case '&'             : return (l && r) ? 1 : 0;
      case '|'             : return (l || r) ? 1 : 0;
      case '<'             : return (l <  r) ? 1 : 0;
      case '>'             : return (l >  r) ? 1 : 0;
      case ArtiToken::Eq   : return (l == r) ? 1 : 0;
      case ArtiToken::Neq  : return (1 != r) ? 1 : 0;
      case ArtiToken::Lte  : return (l <= r) ? 1 : 0;
      case ArtiToken::Gte  : return (l >= r) ? 1 : 0;
      default              : return QVariant();
   }
}

//-----------------------------------------------------------------------------
int Engine::toInt ( ArtiNodePtr n)
{
   QString value = n->text;
   if (value.startsWith("0b")) return value.mid(2).toInt(nullptr, 2);
   else return value.toInt();
}

QVariant Engine::doFunc (const QString& name, QVariantList& args, const quint32 line, const quint32 pos)
{
   int num_args = args.size();

   // check for user function first
   QJSValue function = m_jsengine.globalObject().property(name);
   if (function.isCallable())
   {
      QJSValueList jsargs;
      foreach (QVariant arg, args)
         jsargs.append(variant2js(arg));

      return function.call(jsargs).toVariant();
   }

   // math functions
   if (name == "abs" && num_args > 0) {
      if (isReal(args[0])) return qAbs(args[0].toReal());
      else return qAbs(args[0].toInt());
   }
   else if (name == "acos" && num_args > 0) return qAcos(args[0].toReal());
   else if (name == "asin" && num_args > 0) return qAsin(args[0].toReal());
   else if (name == "atan" && num_args > 0) return qAtan(args[0].toReal());
   else if (name == "cos"  && num_args > 0) return qCos(args[0].toReal());
   else if (name == "exp"  && num_args > 0) return qExp(args[0].toReal());
   else if (name == "ln"   && num_args > 0) return qLn(args[0].toReal());
   else if (name == "max"  && num_args > 1) {
      if (isReal(args[0]) || isReal(args[1]))
         return qMax(args[0].toReal(), args[1].toReal());
      else
         return qMax(args[0].toInt(), args[1].toInt());
   }
   else if (name == "min"  && num_args > 1) {
      if (isReal(args[0]) || isReal(args[1]))
         return qMin(args[0].toReal(), args[1].toReal());
      else
         return qMin(args[0].toInt(), args[1].toInt());
   }
   else if (name == "round" && num_args > 0) return qRound(args[0].toReal());
   else if (name == "sin"  && num_args > 0) return qSin(args[0].toReal());
   else if (name == "sqrt"  && num_args > 0) return qSqrt(args[0].toReal());
   else if (name == "tan"  && num_args > 0) return qTan(args[0].toReal());

   // general functions
   else if (name == "append") {
      if (num_args > 1) {
         if (isArray(args[0])) {
            QVariantList list = args[0].toList();
            list.append(args[1]);
            return list;
         }
         else if (isString(args[0])) return (args[0].toString().append(args[1].toString()));
      }
      else return QVariant();
   }
   else if (name == "camel" && num_args > 0) {
      if (isString(args[0])) {
         QString value = args[0].toString();
         bool word = true;
         for (int index = 0; index < value.length(); index++) {
            if (word && !value[index].isSpace()) {
               value[index] = value[index].toUpper();
               word = false;
            }
            else word = (value[index].isSpace());
         }
         return value;
      }
      else return QVariant();
   }
   else if (name == "cap") {
      if (num_args > 0) {
         QString rtn = args[0].toString();
         if (!rtn.isEmpty()) rtn[0] = rtn[0].toUpper();
         return rtn;
      }
      return QVariant();
   }
   else if (name == "contains") {
      if (num_args > 1) {
         QString key = args[1].toString();
         if (isArray(args[0])) return (args[0].toStringList().contains(key)) ? 1 : 0;
         else if (isMap(args[0])) return (args[0].toMap().contains(key)) ? 1 : 0;
         else if (isString(args[0])) return (args[0].toString().contains(key)) ? 1 : 0;
      }
      else return QVariant();
   }
   else if (name == "date") {
      if (num_args > 0) return QDate::currentDate().toString(args[0].toString());
      else return QVariant();
   }
   else if (name == "dquote") {
      if (num_args > 0 && isString(args[0])) return QString("\"%1\"").arg(args[0].toString());
      else return QVariant();
   }
   else if (name == "empty") {
      if (num_args > 0) {
         if (isArray(args[0])) return (args[0].toList().isEmpty()) ? 1 : 0;
         else if (isMap(args[0])) return (args[0].toMap().isEmpty()) ? 1 : 0;
         else if (isString(args[0])) return (args[0].toString().isEmpty()) ? 1 : 0;
      }
      else return QVariant();
   }
   else if (name == "find") {
      QVariantList results;
      if (args.size() > 1) {
         QVariant context = args[0];
         QString key = args[1].toString();
         if (isArray(context)) results.append(searchArray(context, key));
         else if (isMap(context)) results.append(searchMap(context, key));
      }
      return results;
   }
   else if (name == "first") {
      if (num_args > 0) {
         if (isArray(args[0])) return (args[0].toStringList().first());
         else if (isMap(args[0])) return (args[0].toMap().first());
         else if (isString(args[0])) {
            if (num_args > 2) return (args[0].toString().mid(args[1].toInt(), args[2].toInt()));
            else if (num_args > 1) return (args[0].toString().first(args[1].toInt()));
            else return (args[0].toString().first(1));
         }
      }
      else return QVariant();
   }
   else if (name == "html") {
      if (num_args > 0 && isString(args[0])) return args[0].toString().toHtmlEscaped();
      else return QVariant();
   }
   else if (name == "insert") {
      if (num_args > 2) {
         if (isArray(args[0])) {
            QVariantList list = args[0].toList();
            int index = args[1].toInt();
            if (index >= 0 && index < list.size()) list.insert(index, args[2]);
            return list;
         }
         else if (isString(args[0])) {
            QString text = args[0].toString();
            int index = args[1].toInt();
            if (index >= 0 && index < text.length()) text.insert(index, args[2].toString());
            return text;
         }
         else if (isMap(args[0])) {
            QVariantMap map = args[0].toMap();
            QString key = args[1].toString();
            map.insert(key, args[2]);
            return map;
         }
      }
      else return QVariant();
   }
   else if (name == "isarray") {
      if (num_args > 0) return (isArray(args[0])) ? 1 : 0;
      else return QVariant();
   }
   else if (name == "isbool") {
      if (num_args > 0) return (isBool(args[0])) ? 1 : 0;
      else return QVariant();
   }
   else if (name == "isint") {
      if (num_args > 0) return (isInt(args[0])) ? 1 : 0;
      else return QVariant();
   }
   else if (name == "ismap") {
      if (num_args > 0) return (isMap(args[0])) ? 1 : 0;
      else return QVariant();
   }
   else if (name == "isnull") {
      if (num_args > 0) return (isNull(args[0])) ? 1 : 0;
      else return QVariant();
   }
   else if (name == "isreal") {
      if (num_args > 0) return (isReal(args[0])) ? 1 : 0;
      else return QVariant();
   }
   else if (name == "isstring") {
      if (num_args > 0) return (isString(args[0])) ? 1 : 0;
      else return QVariant();
   }
   else if (name == "join") {
      if (num_args > 1 && isArray(args[0])) return (args[0].toStringList().join(args[1].toString()));
      else return QVariant();
   }
   else if (name == "keys") {
      if (num_args > 0 && isMap(args[0])) return args[0].toMap().keys();
      else return QVariantList();
   }
   else if (name == "last") {
      if (num_args > 0) {
         if (isArray(args[0])) return (args[0].toStringList().last());
         else if (isMap(args[0])) return (args[0].toMap().last());
         else if (isString(args[0])) {
            if (num_args > 1) return (args[0].toString().last(args[1].toInt()));
            else return (args[0].toString().last(1));
         }
      }
      else return QVariant();
   }
   else if (name == "line") {
      return m_doc->blockCount();
   }
   else if (name == "lower") {
      if (num_args > 0 && isString(args[0])) return args[0].toString().toLower();
      else return QVariant();
   }
   else if (name == "prepend") {
      if (num_args > 1) {
         if (isArray(args[0])) {
            QVariantList list = args[0].toList();
            list.prepend(args[1]);
            return list;
         }
         else if (isString(args[0])) return (args[0].toString().prepend(args[1].toString()));
      }
      else return QVariant();
   }
   else if (name == "random") {
      if (num_args == 0) return QRandomGenerator::global()->generate();
      else if (num_args == 1) return QRandomGenerator::global()->bounded(args[0].toInt());
      else if (num_args > 1)  return QRandomGenerator::global()->bounded(args[0].toInt(), args[1].toInt());
      else return QVariant();
   }
   else if (name == "select") {
      if (num_args > 2) return (args[0].toBool()) ? args[1] : args[2];
      else return QVariant();
   }
   else if (name == "size") {
      if (num_args > 0) {
         if (isArray(args[0])) return args[0].toList().length();
         else if (isMap(args[0])) return args[0].toMap().size();
         else if (isString(args[0])) return args[0].toString().length();
      }
      else return QVariant();
   }
   else if (name == "split") {
      if (num_args > 1 && isString(args[0])) return args[0].toString().split(args[1].toString());
      else return QVariant();
   }
   else if (name == "time") {
      if (num_args > 0) return QTime::currentTime().toString(args[0].toString());
      else return QVariant();
   }
   else if (name == "unquote") {
      if (num_args > 0 && isString(args[0])) return args[0].toString().toLatin1();
      else return QVariant();
   }
   else if (name == "squote") {
      if (num_args > 0 && isString(args[0])) return QString("'%1'").arg(args[0].toString());
      else return QVariant();
   }
   else if (name == "upper") {
      if (num_args > 0 && isString(args[0])) return args[0].toString().toUpper();
      else return QVariant();
   }
   else if (name == "words") {
      if (num_args > 0 && isString(args[0])) return args[0].toString().split(QRegularExpression("\\s+")).size();
   }

   else handleMessage(SysWarning, QString("Function %1 is undefined").arg(name), m_filenames.last(), line, pos);

   return QVariant();
}

//-----------------------------------------------------------------------------
void Engine::writeText (const QString& text)
{
   if (m_rtf) m_doc->addHtml(text);
   else if (m_markdown) m_doc->addMarkdown(text);
   else {
      QString last_char = m_doc->lastChar();

      // check for start of new line and handle indentations
      if ((last_char.isEmpty()) || (m_doc->lastChar() == "\n")) {
         int indent = 0;
         QString line;
         bool leading = true;
         for (int index = 0; index < text.size(); index++) {
            QChar c = text[index];
            if (leading && c.isSpace() && c != '\r' && c != '\n') {
               indent++;
            }
            else {
               leading = false;
               line.append(c);
            }
         }

         if (indent != m_indent) {
            m_doc->setIndentDelta(indent - m_indent);
            m_indent += (indent-m_indent);
         }
         m_doc->addText(line);
      }

      // not new line
      else
         m_doc->addText(text);
   }
}
bool Engine::isArray (const QVariant& value) const {return value.typeId() == QMetaType::QVariantList || value.typeId() == QMetaType::QStringList;}
bool Engine::isInt (const QVariant& value) const {return value.typeId() == QMetaType::LongLong || value.typeId() == QMetaType::Int;}
bool Engine::isMap (const QVariant& value) const {return value.typeId() == QMetaType::QVariantMap;}
bool Engine::isNull (const QVariant& value) const {return value.typeId() == QMetaType::Nullptr || value.isNull();}
bool Engine::isReal (const QVariant& value) const {return value.typeId() == QMetaType::QReal;}
bool Engine::isString (const QVariant& value) const {return value.typeId() == QMetaType::QString;}
bool Engine::isBool (const QVariant& value) const {return value.typeId() == QMetaType::Bool;}
QString Engine::toString (const QVariant& value)
{
   if (isArray(value)) {
      QStringList fields;
      foreach (QVariant v, value.toList())
         fields.append(toString(v));
      return QString("[%1]").arg(fields.join(", "));
   }
   else if (isBool(value)) return (value.toBool()) ? "true" : "false";
   else if (isMap(value)) {
      QStringList fields;
      QVariantMap map = value.toMap();
      foreach (QString key, map.keys())
         fields.append(QString("\"%1\":%2").arg(key).arg(toString(map.value(key))));
      return QString("{%1}").arg(fields.join(", "));
   }
   else if (isNull(value)) return "null";
   else if (isString(value)) {return QString("\"%1\"").arg(value.toString());}
   else return value.toString();
}

QString Engine::findFile (QString filename) const
{
   // check if absolute path
   if (QFile::exists(filename)) return filename;

   // look in all defined search paths
   foreach (QString path, m_incpaths)      // assumes paths are absolute
   {
      QString fn = path + "/" + filename;
      if (QFile::exists(fn))
         return fn;
   }
   return QString();
}

QJSValue Engine::variant2js (QVariant var)
{
   if (isArray(var))
   {
      QVariantList vlist = var.toList();
      QJSValue array = m_jsengine.newArray(vlist.size());
      for (int i = 0; i < vlist.size(); i++)
         array.setProperty(i, variant2js(vlist.at(i)));

      return array;
   }
   else if (isMap(var))
   {
      QJSValue map;
      QVariantMap vmap = var.toMap();
      foreach (QString key, vmap.keys())
         map.setProperty(key, variant2js(vmap.value(key)));

      return map;
   }
   else if (isString(var))
      return QJSValue(var.toString());
   else if (isReal(var))
      return QJSValue(var.toReal());
   else if (isInt(var))
      return QJSValue(var.toInt());
   else if (isBool(var))
      return QJSValue(var.toBool());

   return QJSValue();
}
//-----------------------------------------------------------------------------
QVariantList Engine::searchArray (QVariant context, const QString& key)
{
   QVariantList results;
   foreach (QVariant item, context.toList())
      if (isArray(item)) results.append(searchArray(item, key));
      else if (isMap(item)) results.append(searchMap(item, key));
   return results;
}
QVariantList Engine::searchMap (QVariant context, const QString& key)
{
   QVariantList results;
   QVariantMap map = context.toMap();
   foreach (QString k, context.toMap().keys()) {
      QVariant v = map.value(k);
      if      (k == key) results.append(v);
      else if (isArray(v)) results.append(searchArray(v, key));
      else if (isMap(v)) results.append(searchMap(v, key));
   }
   return results;
}
