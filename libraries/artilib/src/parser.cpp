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
#include "parser.h"

#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>

#define TAG_START  "'{%'"
#define TAG_END    "'%}'"
#define EVAL_START "'{{'"
#define EVAL_END   "'}}'"

#define LOC       next_lex().line, next_lex().col

using namespace artiLib;

const QMap<quint32, quint32> Parser::m_prec = {{0,0},
                                               {'<',2}, {'>',2}, {ArtiToken::Lte,2}, {ArtiToken::Gte,2}, {ArtiToken::Eq,2}, {ArtiToken::Neq,2}, {ArtiToken::EqTilde,2},
                                               {'&',3}, {'|',3}, {'^',3},
                                               {'+',4}, {'-',4},
                                               {'*',5}, {'/',5},
                                               {'!',6}, {ArtiToken::Minus,6}
                                              };

Parser::Parser () : QObject() {}
//-----------------------------------------------------------------------------
ArtiDataPtr Parser::parse (const QString& filename)
{
   m_artiData     = std::make_shared<ArtiData>();
   m_artiData->filename = filename;

   m_templateNode = std::make_shared<ArtiNode>(ArtiToken::Template, QFileInfo(filename).baseName(), 0, 0);
   m_artiData->root->children.append(m_templateNode);

   QFile file(filename);
   if (!file.open(QIODevice::ReadOnly)) {
      MESSAGE(SysError, QString("Cannot open file %1 for reading").arg(filename));
      m_status = false;
   }
   else {
      m_stream.setDevice(&file);
      m_line  = 1;
      m_col   = 0;
      tokenize();
      file.close();

      // parse
      while (m_status && next_lex().token != ArtiToken::End) {
         if (next_lex().token == ArtiToken::CtagStart)
            m_templateNode->children.append(parse_ctag());
         else
            m_templateNode->children.append(parse_element());
      }

      // QFile out("arti_debug.dot");
      // if (!out.open(QIODevice::WriteOnly)) {
      //    MESSAGE(ErrorMessage, QString("Cannot open file %1 for writing").arg(QFileInfo(out).absoluteFilePath()));
      // }
      // else {
      //    qDebug() << "dot file: " << QFileInfo(out).absoluteFilePath();
      //    QTextStream out_stream(&out);
      //    parserLib::nodeWriter writer;
      //    writer.writeDot(_rootNode, out_stream);
      // }
   }

   return m_artiData;
}
bool Parser::status () const {return m_status;}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// parser functions
//-----------------------------------------------------------------------------
// parse text, tag or eval
ArtiNodePtr Parser::parse_element ()
{
   if (next_lex().token == ArtiToken::Text) {
      ArtiNodePtr textNode = std::make_shared<ArtiNode>(ArtiToken::Text, next_lex().value, LOC);
      consume_lex();
      return textNode;
   }
   else if (next_lex().token == ArtiToken::TagStart)   return parse_tag();
   else if (next_lex().token == ArtiToken::EvalStart)  return parse_eval();

   syntax_error({"<text>", TAG_START, EVAL_START});
   return nullptr;
}
//-----------------------------------------------------------------------------
ArtiNodeList Parser::parse_ctag ()
{
   ArtiNodeList stmts;

   consume_lex();    // ctag start
   while (m_status && next_lex().token != ArtiToken::CtagEnd)
      stmts.append(parse_stmts());

   if (next_lex().token == ArtiToken::CtagEnd) consume_lex();

   return stmts;
}
ArtiNodePtr Parser::parse_stmts ()
{
   if (next_lex().token == ArtiToken::Id) {
      QString name = next_lex().value;
      if      (name == "for")      return parse_for_stmt();
      else if (name == "if")       return parse_if_stmt();
      else if (name == "include")  return parse_include_stmt();
      else if (name == "info")     return parse_info_stmt();
      else if (name == "debug")    return parse_debug_stmt();
      else if (name == "status")   return parse_status_stmt();
      else if (name == "warning")  return parse_warning_stmt();
      else if (name == "error")    return parse_error_stmt();
      else if (name == "fatal")    return parse_fatal_stmt();
      else if (name == "print")    return parse_print_stmt();
      else if (name == "set")      return parse_set_stmt();
      else if (name == "while")    return parse_while_stmt();
   }
   syntax_error({"for", "if", "include", "info", "debug", "status", "warning", "error", "fatal", "print", "set", "while"});
   return nullptr;
}
ArtiNodePtr Parser::parse_for_stmt ()
{
   ArtiNodePtr forNode = std::make_shared<ArtiNode>(ArtiToken::For, LOC);
   consume_lex();
   if (next_lex().token == ArtiToken::Id) {
      forNode->children.append(std::make_shared<ArtiNode>(ArtiToken::Id, next_lex().value, LOC));
      consume_lex();
      if (next_lex().token == ArtiToken::Id && next_lex().value == "in") {
         consume_lex();
         forNode->children.append(parse_expr());
         if (m_status) {
            if (next_lex().token == ArtiToken::DotDot) {
               consume_lex();
               forNode->children.append(parse_expr());
            }
         }
      }
      else syntax_error({"in"});
   } else syntax_error({"<id>"});

   ArtiNodePtr stmtNode = std::make_shared<ArtiNode>(ArtiToken::Stmt, LOC);
   forNode->children.append(stmtNode);

   while (m_status) {
      stmtNode->children.append(parse_stmts());
      if (next_lex().token == ArtiToken::Id && next_lex().value == "endfor") {
         consume_lex();
         break;
      }
   }
   return forNode;
}
ArtiNodePtr Parser::parse_if_stmt ()
{
   ArtiNodePtr ifNode = std::make_shared<ArtiNode>(ArtiToken::If, LOC);
   consume_lex();
   ifNode->children.append(parse_expr());    // condition
   ArtiNodePtr stmtNode = std::make_shared<ArtiNode>(ArtiToken::Stmt, LOC);
   ifNode->children.append(stmtNode);

   while (m_status) {
      if (next_lex().token == ArtiToken::Id) {
         if (next_lex().value == "elsif") {
            ArtiNodePtr elsifNode = std::make_shared<ArtiNode>(ArtiToken::Elsif, LOC);
            consume_lex();
            ifNode->children.append(elsifNode);
            elsifNode->children.append(parse_expr());    // condition
            stmtNode = std::make_shared<ArtiNode>(ArtiToken::Stmt, LOC);
            elsifNode->children.append(stmtNode);
         }
         else if (next_lex().value == "else") {
            ArtiNodePtr elseNode = std::make_shared<ArtiNode>(ArtiToken::Else, LOC);
            consume_lex();
            ifNode->children.append(elseNode);
            stmtNode = std::make_shared<ArtiNode>(ArtiToken::Stmt, LOC);
            elseNode->children.append(stmtNode);
         }
         else if (next_lex().value == "endif") {
            consume_lex();
            return ifNode;
         }
      }
   }
   syntax_error({"endif"});
   return ifNode;
}
ArtiNodePtr Parser::parse_include_stmt ()
{
   ArtiNodePtr incNode = std::make_shared<ArtiNode>(ArtiToken::Include, LOC);
   consume_lex();
   incNode->children.append(parse_expr());
   if (m_status && next_lex().token == ArtiToken::Id && next_lex().value == "with") {
      consume_lex();
      incNode->children.append(parse_assign_list());
   }

   return incNode;
}
ArtiNodePtr Parser::parse_info_stmt ()
{
   ArtiNodePtr msgNode = std::make_shared<ArtiNode>(ArtiToken::Info, LOC);
   consume_lex();
   msgNode->children.append(parse_string());

   return msgNode;
}
ArtiNodePtr Parser::parse_debug_stmt ()
{
   ArtiNodePtr msgNode = std::make_shared<ArtiNode>(ArtiToken::Debug, LOC);
   consume_lex();
   msgNode->children.append(parse_string());

   return msgNode;
}
ArtiNodePtr Parser::parse_status_stmt ()
{
   ArtiNodePtr msgNode = std::make_shared<ArtiNode>(ArtiToken::Status, LOC);
   consume_lex();
   msgNode->children.append(parse_string());

   return msgNode;
}
ArtiNodePtr Parser::parse_warning_stmt ()
{
   ArtiNodePtr msgNode = std::make_shared<ArtiNode>(ArtiToken::Warning, LOC);
   consume_lex();
   msgNode->children.append(parse_string());

   return msgNode;
}
ArtiNodePtr Parser::parse_error_stmt ()
{
   ArtiNodePtr msgNode = std::make_shared<ArtiNode>(ArtiToken::Error, LOC);
   consume_lex();
   msgNode->children.append(parse_string());

   return msgNode;
}
ArtiNodePtr Parser::parse_fatal_stmt ()
{
   ArtiNodePtr msgNode = std::make_shared<ArtiNode>(ArtiToken::Fatal, LOC);
   consume_lex();
   msgNode->children.append(parse_string());

   return msgNode;
}
ArtiNodePtr Parser::parse_print_stmt ()
{
   ArtiNodePtr printNode = std::make_shared<ArtiNode>(ArtiToken::Print, LOC);
   consume_lex();
   printNode->children.append(parse_string());

   return printNode;
}
ArtiNodePtr Parser::parse_set_stmt ()
{
   ArtiNodePtr setNode = std::make_shared<ArtiNode>(ArtiToken::Set, LOC);
   consume_lex();
   setNode->children.append(parse_assign_list());

   return setNode;
}
ArtiNodePtr Parser::parse_while_stmt ()
{
   ArtiNodePtr whileNode = std::make_shared<ArtiNode>(ArtiToken::While, LOC);
   consume_lex();
   whileNode->children.append(parse_expr());

   ArtiNodePtr stmtNode = std::make_shared<ArtiNode>(ArtiToken::Stmt, LOC);
   whileNode->children.append(stmtNode);

   while (m_status) {
      stmtNode->children.append(parse_stmts());
      if (next_lex().token == ArtiToken::Id && next_lex().value == "endwhile") {
         consume_lex();
         break;
      }
   }
   return whileNode;
}
//-----------------------------------------------------------------------------
ArtiNodePtr Parser::parse_text ()
{
   ArtiNodePtr textNode = std::make_shared<ArtiNode>(ArtiToken::Text, next_lex().value, LOC);
   consume_lex();
   return textNode;
}

ArtiNodePtr Parser::parse_tag()
{
   consume_lex();    // tag start
   if (next_lex().token == ArtiToken::Id) {
      QString name = next_lex().value;
      if      (name == "align")    return parse_align();
      else if (name == "for")      return parse_for();
      else if (name == "if")       return parse_if();
      else if (name == "include")  return parse_include();
      else if (name == "load")     return parse_load();
      else if (name == "info")     return parse_info();
      else if (name == "debug")    return parse_debug();
      else if (name == "status")   return parse_status();
      else if (name == "warning")  return parse_warning();
      else if (name == "error")    return parse_error();
      else if (name == "fatal")    return parse_fatal();
      else if (name == "print")    return parse_print();
      else if (name == "set")      return parse_set();
      else if (name == "template") return parse_template();
      else if (name == "while")    return parse_while();
   }
   syntax_error({"align", "for", "if", "include", "load", "info", "debug", "status", "warning", "error", "fatal", "print", "set", "template", "while"});
   return nullptr;
}

ArtiNodePtr Parser::parse_eval ()
{
   ArtiNodePtr evalNode = std::make_shared<ArtiNode>(ArtiToken::Eval, LOC);
   consume_lex();

   // parse expression
   evalNode->children.append(parse_expr());

   if (next_lex().token == ArtiToken::EvalEnd) consume_lex();
   else syntax_error({EVAL_END});

   return evalNode;
}

//-----------------------------------------------------------------------------
ArtiNodePtr Parser::parse_align ()
{
   ArtiNodePtr alignNode = std::make_shared<ArtiNode>(ArtiToken::Align, LOC);
   consume_lex();
   alignNode->children.append(parse_expr());

   if (next_lex().token == ArtiToken::TagEnd) consume_lex();
   else syntax_error({TAG_END});

   return alignNode;
}

ArtiNodePtr Parser::parse_for ()
{
   ArtiNodePtr forNode = std::make_shared<ArtiNode>(ArtiToken::For, LOC);
   consume_lex();
   if (next_lex().token == ArtiToken::Id) {
      forNode->children.append(std::make_shared<ArtiNode>(ArtiToken::Id, next_lex().value, LOC));
      consume_lex();
      if (next_lex().token == ArtiToken::Id && next_lex().value == "in") {
         consume_lex();
         forNode->children.append(parse_expr());
         if (m_status) {
            if (next_lex().token == ArtiToken::DotDot) {
               consume_lex();
               forNode->children.append(parse_expr());
            }
         }
      }
      else syntax_error({"in"});
   } else syntax_error({"<id>"});

   if (next_lex().token == ArtiToken::TagEnd) {
      consume_lex();

      // get elements until the next endfor
      ArtiNodePtr stmtNode = std::make_shared<ArtiNode>(ArtiToken::Stmt, LOC);
      forNode->children.append(stmtNode);
      ArtiNodePtr elemNode = parse_element();
      while (m_status) {
         stmtNode->children.append(elemNode);
         if (next_lex().token == ArtiToken::TagStart && look_ahead().token == ArtiToken::Id && look_ahead().value == "endfor") {
            consume_lex(); // tag start
            consume_lex(); // endfor
            if (next_lex().token == ArtiToken::TagEnd) {
               consume_lex();
               break;
            }
            else syntax_error({TAG_END});
         }
         else elemNode = parse_element();
      }
   }
   else syntax_error({TAG_END});
   return forNode;
}

ArtiNodePtr Parser::parse_if ()
{
   ArtiNodePtr ifNode = std::make_shared<ArtiNode>(ArtiToken::If, LOC);
   consume_lex();
   ifNode->children.append(parse_expr());    // condition
   if (next_lex().token == ArtiToken::TagEnd) {
      consume_lex();
      ArtiNodePtr stmtNode = std::make_shared<ArtiNode>(ArtiToken::Stmt, LOC);
      ifNode->children.append(stmtNode);

      // get elements until the next elsif, else or endif
      ArtiNodePtr elemNode = parse_element();
      while (m_status) {
         stmtNode->children.append(elemNode);

         // check for elsif
         if (next_lex().token == ArtiToken::TagStart && look_ahead().token == ArtiToken::Id && look_ahead().value == "elsif") {
            consume_lex();    // tag start
            ArtiNodePtr elsifNode = std::make_shared<ArtiNode>(ArtiToken::Elsif, LOC);
            consume_lex();    // elsif
            ifNode->children.append(elsifNode);
            elsifNode->children.append(parse_expr());
            if (next_lex().token == ArtiToken::TagEnd) {
               consume_lex();
               stmtNode = std::make_shared<ArtiNode>(ArtiToken::Stmt, LOC);
               elsifNode->children.append(stmtNode);
               elemNode = parse_element();
            }
            else syntax_error({TAG_END});
         }

         // check for else
         else if (next_lex().token == ArtiToken::TagStart && look_ahead().token == ArtiToken::Id && look_ahead().value == "else") {
            consume_lex();    // tag start
            stmtNode = std::make_shared<ArtiNode>(ArtiToken::Else, LOC);
            consume_lex();    // else
            ifNode->children.append(stmtNode);
            if (next_lex().token == ArtiToken::TagEnd) {
               consume_lex();
               elemNode = parse_element();
            }
            else syntax_error({TAG_END});
         }

         // check for endif
         else if (next_lex().token == ArtiToken::TagStart && look_ahead().token == ArtiToken::Id && look_ahead().value == "endif") {
            consume_lex();    // tag start
            consume_lex();    // endif
            if (next_lex().token == ArtiToken::TagEnd) {
               consume_lex();
               break;
            }
            else syntax_error({TAG_END});
         }
         else elemNode = parse_element();
      }
   }
   else syntax_error({TAG_END});

   return ifNode;
}

ArtiNodePtr Parser::parse_include ()
{
   ArtiNodePtr incNode = std::make_shared<ArtiNode>(ArtiToken::Include, LOC);
   consume_lex();
   incNode->children.append(parse_expr());
   if (m_status && next_lex().token == ArtiToken::Id && next_lex().value == "with") {
      consume_lex();
      incNode->children.append(parse_assign_list());
   }

   if (next_lex().token == ArtiToken::TagEnd) consume_lex();
   else syntax_error({TAG_END});

   return incNode;
}

ArtiNodePtr Parser::parse_load ()
{
   ArtiNodePtr loadNode = std::make_shared<ArtiNode>(ArtiToken::Load, LOC);
   consume_lex();
   loadNode->children.append(parse_expr());

   if (next_lex().token == ArtiToken::TagEnd) consume_lex();
   else syntax_error({TAG_END});

   return loadNode;
}

ArtiNodePtr Parser::parse_info ()
{
   ArtiNodePtr msgNode = std::make_shared<ArtiNode>(ArtiToken::Info, LOC);
   consume_lex();
   msgNode->children.append(parse_string());

   if (m_status) {
      if (next_lex().token == ArtiToken::TagEnd) consume_lex();
      else syntax_error({TAG_END});
   }

   return msgNode;
}

ArtiNodePtr Parser::parse_debug ()
{
   ArtiNodePtr msgNode = std::make_shared<ArtiNode>(ArtiToken::Debug, LOC);
   consume_lex();
   msgNode->children.append(parse_string());

   if (m_status) {
      if (next_lex().token == ArtiToken::TagEnd) consume_lex();
      else syntax_error({TAG_END});
   }

   return msgNode;
}
ArtiNodePtr Parser::parse_status ()
{
   ArtiNodePtr msgNode = std::make_shared<ArtiNode>(ArtiToken::Status, LOC);
   consume_lex();
   msgNode->children.append(parse_string());

   if (m_status) {
      if (next_lex().token == ArtiToken::TagEnd) consume_lex();
      else syntax_error({TAG_END});
   }

   return msgNode;
}
ArtiNodePtr Parser::parse_warning ()
{
   ArtiNodePtr msgNode = std::make_shared<ArtiNode>(ArtiToken::Warning, LOC);
   consume_lex();
   msgNode->children.append(parse_string());

   if (m_status) {
      if (next_lex().token == ArtiToken::TagEnd) consume_lex();
      else syntax_error({TAG_END});
   }

   return msgNode;
}
ArtiNodePtr Parser::parse_error ()
{
   ArtiNodePtr msgNode = std::make_shared<ArtiNode>(ArtiToken::Error, LOC);
   consume_lex();
   msgNode->children.append(parse_string());

   if (m_status) {
      if (next_lex().token == ArtiToken::TagEnd) consume_lex();
      else syntax_error({TAG_END});
   }

   return msgNode;
}
ArtiNodePtr Parser::parse_fatal ()
{
   ArtiNodePtr msgNode = std::make_shared<ArtiNode>(ArtiToken::Fatal, LOC);
   consume_lex();
   msgNode->children.append(parse_string());

   if (m_status) {
      if (next_lex().token == ArtiToken::TagEnd) consume_lex();
      else syntax_error({TAG_END});
   }

   return msgNode;
}
ArtiNodePtr Parser::parse_print ()
{
   // PRINT SSTRING
   // PRINT DSTRING (, expr)*
   ArtiNodePtr printNode = std::make_shared<ArtiNode>(ArtiToken::Print, LOC);
   consume_lex();
   printNode->children.append(parse_string());

   if (m_status) {
      if (next_lex().token == ArtiToken::TagEnd) consume_lex();
      else syntax_error({TAG_END});
   }

   return printNode;
}

ArtiNodePtr Parser::parse_set ()
{
   ArtiNodePtr setNode = std::make_shared<ArtiNode>(ArtiToken::Set, LOC);
   consume_lex();
   setNode->children.append(parse_assign_list());

   if (next_lex().token == ArtiToken::TagEnd) consume_lex();
   else syntax_error({TAG_END});

   return setNode;
}

ArtiNodePtr Parser::parse_template ()
{
   m_templateNode = std::make_shared<ArtiNode>(ArtiToken::Template, LOC);
   consume_lex();
   if (next_lex().token == ArtiToken::SString || next_lex().token == ArtiToken::DString) {
      m_templateNode->text = next_lex().value;
      consume_lex();
      if (next_lex().token == ArtiToken::TagEnd) consume_lex();
      else syntax_error({TAG_END});
   } else syntax_error({"<dstring>", "<sstring>"});

   m_artiData->root->children.append(m_templateNode);
   return nullptr;
}
ArtiNodePtr Parser::parse_while ()
{
   ArtiNodePtr whileNode = std::make_shared<ArtiNode>(ArtiToken::While, LOC);
   consume_lex();
   whileNode->children.append(parse_expr());

   if (next_lex().token == ArtiToken::TagEnd) {
      consume_lex();

      // get elements until the next endfor
      ArtiNodePtr stmtNode = std::make_shared<ArtiNode>(ArtiToken::Stmt, LOC);
      whileNode->children.append(stmtNode);
      ArtiNodePtr elemNode = parse_element();
      while (m_status) {
         stmtNode->children.append(elemNode);
         if (next_lex().token == ArtiToken::TagStart && look_ahead().token == ArtiToken::Id && look_ahead().value == "endwhile") {
            consume_lex(); // tag start
            consume_lex(); // endwhile
            if (next_lex().token == ArtiToken::TagEnd) {
               consume_lex();
               break;
            }
            else syntax_error({TAG_END});
         }
         else elemNode = parse_element();
      }
   }
   else syntax_error({TAG_END});
   return whileNode;
}
//-------------------------------------------------------------------
ArtiNodeList Parser::parse_assign_list ()
{
   ArtiNodeList list;
   // ID '=' struct (',' ID '=' struct)*
   while (m_status) {
      if (next_lex().token == ArtiToken::Id) {
         ArtiNodePtr assignNode = std::make_shared<ArtiNode>(ArtiToken::Assign, LOC);
         list.append(assignNode);
         assignNode->children.append(parse_id());
         if (next_lex().token == '=') {
            consume_lex();
            assignNode->children.append(parse_struct());
            if (next_lex().token == ',') consume_lex();
            else break;
         }
         else syntax_error({"'='"});
      }
      else syntax_error({"<id>"});
   }

   return list;
}

ArtiNodePtr Parser::parse_struct ()
{
   if      (next_lex().token == '[') {return parse_array();}
   else if (next_lex().token == '{') {return parse_map();}
   else                              return parse_expr();
}

ArtiNodePtr Parser::parse_array ()
{
   ArtiNodePtr arrayNode = std::make_shared<ArtiNode>(ArtiToken::Array, LOC);
   consume_lex();
   if (next_lex().token == ']')     // empty array
      consume_lex();
   else {
      while (m_status) {
         arrayNode->children.append(parse_struct());
         if (next_lex().token == ',') consume_lex();
         else if (next_lex().token == ']') {
            consume_lex();
            break;
         }
         else syntax_error({"','", "']'"});
      }
   }
   return arrayNode;
}
ArtiNodePtr Parser::parse_map ()
{
   ArtiNodePtr mapNode = std::make_shared<ArtiNode>(ArtiToken::Map, LOC);
   consume_lex();
   if (next_lex().token == '}')     // empty map
      consume_lex();
   else {
      while (m_status) {
         mapNode->children.append(parse_expr());            // key
         if (m_status && next_lex().token == ':') {
            consume_lex();
            mapNode->children.append(parse_struct());       // value
            if (next_lex().token == ',') consume_lex();
            else if (next_lex().token == '}') {
               consume_lex();
               break;
            }
            else syntax_error({"','", "'}'"});
         }
         else if (m_status) syntax_error({"':'"});
      }
   }
   return mapNode;
}

//-------------------------------------------------------------------
// functions to parse an expression
//-------------------------------------------------------------------
ArtiNodePtr Parser::parse_expr()
{
   // initialize
   m_opStack.clear();
   m_nodeStack.clear();
   m_opStack.push({0, QString(), 0, 0});

   do_expr();

   ArtiNodePtr exprNode = (m_status) ? m_nodeStack.pop() : nullptr;

   return exprNode;
}

ArtiNodePtr Parser::parse_id ()
{
   // ID ('.' field)*

   // field
      // ID
      // INTEGER
      // '(' expr ')'

   // assumes on entry current lex is tId
    ArtiNodePtr idNode = std::make_shared<ArtiNode>(ArtiToken::Id, next_lex().value, LOC);
    consume_lex();
    while (m_status && next_lex().token == '.') {
      consume_lex();
      if (next_lex().token == ArtiToken::Id) {
         idNode->children.append(std::make_shared<ArtiNode>(ArtiToken::Id, next_lex().value, LOC));
         consume_lex();
      }
      else if (m_status && next_lex().token == '(') {
         consume_lex();
         idNode->children.append(parse_expr());
         if (next_lex().token == ')')
            consume_lex();
         else syntax_error({"')'"});
      }
      else idNode->children.append(parse_integer());
    }
    return idNode;
}
ArtiNodePtr Parser::parse_sysid ()
{
   ArtiNodePtr sysidNode = std::make_shared<ArtiNode>(ArtiToken::Sysid, next_lex().value, LOC);
   consume_lex();
   while (m_status && next_lex().token == '.') {
      consume_lex();
      if (next_lex().token == ArtiToken::Id) {
         sysidNode->children.append(std::make_shared<ArtiNode>(ArtiToken::Id, next_lex().value, LOC));
         consume_lex();
      }
      else sysidNode->children.append(parse_integer());
   }
   return sysidNode;
}
ArtiNodePtr Parser::parse_number ()
{
   ArtiNodePtr numNode;
   if (next_lex().token == ArtiToken::Real) {
      numNode = std::make_shared<ArtiNode>(ArtiToken::Real, next_lex().value, LOC);
      consume_lex();
   }
   else
      numNode = parse_integer();
   return numNode;
}
ArtiNodePtr Parser::parse_integer ()
{
   ArtiNodePtr intNode = nullptr;
   if (next_lex().token == ArtiToken::Hex || next_lex().token == ArtiToken::Dec || next_lex().token == ArtiToken::Oct || next_lex().token == ArtiToken::Bin) {
      intNode = std::make_shared<ArtiNode>(ArtiToken::Integer, next_lex().value, LOC);
      consume_lex();
   }
   else syntax_error({"<integer>"});

   return intNode;
}
ArtiNodePtr Parser::parse_func ()
{
   // ID '(' (expr (',' expr)*)? ')'
   // assumes current lex is tId
   // assumes look-ahead was performed to ensure tId followed by '(', so no need to check
   ArtiNodePtr funcNode = (std::make_shared<ArtiNode>(ArtiToken::Func, next_lex().value, LOC));
   consume_lex();
   consume_lex();    // consume '('
   if (next_lex().token == ')')     // no function arguments
      consume_lex();
   else {
      while (m_status) {
         funcNode->children.append(parse_struct());
         if (next_lex().token == ',') consume_lex();
         else if (next_lex().token == ')') {
            consume_lex();
            break;
         }
         else syntax_error({"','", "')'"});
      }
   }

   return funcNode;
}
ArtiNodeList Parser::parse_string ()
{
   ArtiNodeList rtn;
   bool sstring = next_lex().token == ArtiToken::SString;
   bool dstring = next_lex().token == ArtiToken::DString;

   if (!sstring && !dstring) {
      syntax_error({"<dstring>", "<sstring>"});
      return rtn;
   }

   // parse string
   rtn.append(parse_expr());

   if (dstring) {
      while (m_status && next_lex().token == ',') {
         consume_lex();
         rtn.append(parse_expr());
      }
   }
   return rtn;
}
//-------------------------------------------------------------------
void Parser::do_expr ()
{
   do_primary();

   while (m_status && m_prec.contains(next_lex().token) && m_prec.value(next_lex().token) < 6) {
      push_operator(next_lex());
      consume_lex();
      do_primary();
   }

   // perform any outstanding operations
   while (m_status && m_opStack.last().token != 0)
      do_op();
}

void Parser::do_primary ()
{
   switch (next_lex().token)
   {
      case ArtiToken::Id       :        // could be id or function call
      {
         if (look_ahead().token == '(')      // function call
            m_nodeStack.push(parse_func());
         else
            m_nodeStack.push(parse_id());
         break;
      }
      case ArtiToken::Sysid    : m_nodeStack.push(parse_sysid()); break;
      case ArtiToken::True     :
      {
         m_nodeStack.push(std::make_shared<ArtiNode>(ArtiToken::True, next_lex().value, LOC));
         consume_lex();
         break;
      }
      case ArtiToken::False    :
      {
         m_nodeStack.push(std::make_shared<ArtiNode>(ArtiToken::False, next_lex().value, LOC));
         consume_lex();
         break;
      }
      case ArtiToken::Null     :
      {
         m_nodeStack.push(std::make_shared<ArtiNode>(ArtiToken::Null, next_lex().value, LOC));
         consume_lex();
         break;
      }
      case ArtiToken::Hex      :
      case ArtiToken::Dec      :
      case ArtiToken::Oct      :
      case ArtiToken::Bin      :
      case ArtiToken::Real     : m_nodeStack.push(parse_number()); break;
      case '('       :
      {
         consume_lex();
         m_opStack.push({0, QString(), 0, 0});
         do_expr();
         if (next_lex().token != ')') {
            syntax_error({"')'"});
            return;
         }
         else {
            consume_lex();
            m_opStack.pop();
         }
         break;
      }
      case ArtiToken::SString  : m_nodeStack.push(std::make_shared<ArtiNode>(ArtiToken::SString, next_lex().value, LOC)); consume_lex(); break;
      case ArtiToken::DString  : m_nodeStack.push(std::make_shared<ArtiNode>(ArtiToken::DString, next_lex().value, LOC)); consume_lex(); break;
      case ArtiToken::Regex    : m_nodeStack.push(std::make_shared<ArtiNode>(ArtiToken::Regex, next_lex().value, LOC)); consume_lex(); break;
      case '-'       :
      {
         push_operator({ArtiToken::Minus, QString(), next_lex().line, next_lex().col});
         consume_lex();
         do_primary();
         break;
      }
      case '!'       :
      {
         push_operator({'!', QString(), next_lex().line, next_lex().col});
         consume_lex();
         do_primary();
         break;
      }
      case '+'       :
      {
         consume_lex();  // do nothing with this operator
         do_primary();
         break;
      }
      default : syntax_error({"<primary expr>"});
   }
}

//-------------------------------------------------------------------
// function to perform a binary or unary operation
//-------------------------------------------------------------------
void Parser::do_op ()
{
   if (m_opStack.size() == 0 || m_nodeStack.size() == 0) {
      MESSAGE(SysError, tr("Stack underflow error"), m_artiData->filename, next_lex().line, next_lex().col);
      m_status = false;
      return;
   }

   Lexeme op    = m_opStack.pop();
   ArtiNodePtr right  = m_nodeStack.pop();
   ArtiNodePtr opNode = mk_op_node(op);
   opNode->children.append(right);

   // unary operator
   if (op.token == ArtiToken::Minus || op.token == '!') m_nodeStack.push(opNode);

   // binary operator
   else {
      opNode->children.prepend(m_nodeStack.pop());    // left operand
      m_nodeStack.push(opNode);
   }
}

void Parser::push_operator (Lexeme op)
{
   while (m_prec[m_opStack.last().token] > m_prec[op.token])
      do_op();
   m_opStack.push(op);
}

ArtiNodePtr Parser::mk_op_node (Lexeme op)
{
   int node_token = (op.token == ArtiToken::Eq)       ? ArtiToken::Eq      :
                    (op.token == ArtiToken::Neq)      ? ArtiToken::Neq     :
                    (op.token == ArtiToken::Lte)      ? ArtiToken::Lte     :
                    (op.token == ArtiToken::Gte)      ? ArtiToken::Gte     :
                    (op.token == ArtiToken::EqTilde)  ? ArtiToken::EqTilde :
                    (op.token == ArtiToken::Minus)    ? ArtiToken::Minus   : op.token;

   return std::make_shared<ArtiNode>(node_token, op.line, op.col);
}

//-------------------------------------------------------------------
Parser::Lexeme Parser::next_lex () {return m_lexemes[0];}
void Parser::consume_lex () {m_lexemes.pop_front();}
Parser::Lexeme Parser::look_ahead () {return m_lexemes[1];}

//-----------------------------------------------------------------------------
// lexer functions
//-----------------------------------------------------------------------------
void Parser::tokenize ()
{
   // toeknize states
   typedef enum {
      Text,
      InTagEval,
      Id,
      Sysid,
      Sysnum,
      Hex,
      Dec,
      Oct,
      Bin,
      Fraction,
      Exponent,
      Sstring,
      Dstring,
      Regex,
      Comment
   } states;

   // tokenize variables
   QString text;
   quint32 sline;
   quint32 scol;
   bool    eot    = false;
   states  state  = Text;
   bool    inEval = false;
   bool    inId   = false;

   // tokenize
   next_char();      // load first character
   sline = m_line;
   scol = m_col;
   while (m_status && !m_char.isNull()) {
      switch (state)
      {
         case Text      :
         {
            if (m_char == '{' && m_prevc != '\\' && m_nextc == '%') {         // start of tag
               if (!text.isEmpty())
                  m_lexemes.append({ArtiToken::Text, text, sline, scol});
               m_lexemes.append({ArtiToken::TagStart, "{%", m_line, m_col});
               text.clear();
               sline = m_line;
               scol  =  m_col;
               next_char();         // read past %
               state = InTagEval;
            }
            else if (m_char == '{' && m_prevc != '\\' && m_nextc == '{') {    // start of eval
               if (!text.isEmpty())
                  m_lexemes.append({ArtiToken::Text, text, sline, scol});
               m_lexemes.append({ArtiToken::EvalStart, "{{", m_line, m_col});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               next_char();         // read past {
               inEval = true;
               state = InTagEval;
            }
            else if (m_char == '{' && m_prevc != '\\' && m_nextc == '#') {    // start of comment
               if (!text.isEmpty())
                  m_lexemes.append({ArtiToken::Text, text, sline, scol});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               next_char();         // read past #
               state = Comment;
            }
            else if (m_char == '<' && m_prevc != '\\' && m_nextc == '%') {         // start of complex tag
               if (!text.isEmpty())
                  m_lexemes.append({ArtiToken::Text, text, sline, scol});
               m_lexemes.append({ArtiToken::CtagStart, "<%", m_line,  m_col});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               next_char();         // read past %
               state = InTagEval;
            }
            else {
               text.append(m_char);
               if (m_char == '\n') {     // end of line
                  if (eot && text.contains(QRegularExpression("^\\s*$")))
                     text.clear();
                  else {
                     m_lexemes.append({ArtiToken::Text, text, sline, scol});
                     sline  = m_line;
                     scol   = m_col;
                     text.clear();
                  }
                  eot = false;
               }
            }

            next_char();
            break;
         }

         case InTagEval   :
         {
            // handle escaped characters
            if (m_char == '\\') {     // just save escaped character
               m_lexemes.append({ArtiToken::Text, QString("%1%2").arg(m_char).arg(m_nextc), m_line, m_col});
               next_char();         // read past next character which is already consumed
            }

            // end markers
            else if (m_char == '%' && m_nextc == '}') {          // end of tag
               m_lexemes.append({ArtiToken::TagEnd, "%}", m_line, m_col});
               next_char();      // read past '}'
               sline  = m_line;
               scol   = m_col;
               eot    = true;
               state  = Text;
            }
            else if (m_char == '}' && m_nextc == '}' && inEval) {          // end of eval
               m_lexemes.append({ArtiToken::EvalEnd, "}}", m_line, m_col});
               next_char();      // read past '}'
               sline  = m_line;
               scol   = m_col;
               inEval = false;
               state  = Text;
            }
            else if (m_char == '%' && m_nextc == '>') {          // end of ctag
               m_lexemes.append({ArtiToken::CtagEnd, "%>", m_line, m_col});
               next_char();      // read past '}'
               sline  = m_line;
               scol   = m_col;
               eot    = true;
               state  = Text;
            }

            // words or numbers
            else if (m_char.isLetter() || (m_char == '_' && m_nextc.isLetter())) {
               text.append(m_char);
               sline  = m_line;
               scol   = m_col;
               inId   = true;
               state  = Id;
            }
            else if (m_char == '$' && m_nextc.isLetter()) {
               text.append(m_char);
               sline  = m_line;
               scol   = m_col;
               state  = Sysid;
            }
            else if (m_char == '$' && m_nextc.isDigit()) {
               text.append(m_char);
               sline  = m_line;
               scol   = m_col;
               state  = Sysnum;
            }
            else if (m_char == '0' && m_nextc == 'b') {
               text.append("0b");
               sline  = m_line;
               scol   = m_col;
               next_char();      // read past b
               state  = Bin;
            }
            else if (m_char == '0' && m_nextc == 'o') {
               text.append("0o");
               sline  = m_line;
               scol   = m_col;
               next_char();      // read past o
               state = Oct;
            }
            else if (m_char == '0' && m_nextc == 'x') {
               text.append("0x");
               sline  = m_line;
               scol   = m_col;
               next_char();      // read past x
               state  = Hex;
            }
            else if (m_char.isDigit()) {
               text.append(m_char);
               sline  = m_line;
               scol   = m_col;
               state  = Dec;
            }

            // 2-character operators
            else if (m_char == '=' && m_nextc == '=') {
               m_lexemes.append({ArtiToken::Eq, "==", m_line, m_col});
               next_char();   // read past second '='
            }
            else if (m_char == '!' && m_nextc == '=') {
               m_lexemes.append({ArtiToken::Neq, "!=", m_line, m_col});
               next_char();   // read past second '='
            }
            else if (m_char == '>' && m_nextc == '=') {
               m_lexemes.append({ArtiToken::Gte, ">=", m_line, m_col});
               next_char();   // read past '='
            }
            else if (m_char == '<' && m_nextc == '=') {
               m_lexemes.append({ArtiToken::Lte, "<=", m_line, m_col});
               next_char();   // read past '='
            }
            else if (m_char == '.' && m_nextc == '.') {
               m_lexemes.append({ArtiToken::DotDot, "..", m_line, m_col});
               next_char();   // read past second '.'
            }
            else if (m_char == '=' && m_nextc == '~') {
               m_lexemes.append({ArtiToken::EqTilde, "=~", m_line, m_col});
               next_char();   // read past '~'
               // read past whitespace
               skip_ws();

               // check for start of regex
               if (m_char == '/') state = Regex;
            }
            else if (m_char == '"') state = Dstring;
            else if (m_char == '\'') state = Sstring;

            // single character
            else if (!m_char.isSpace()) {   // skip whitespace
               m_lexemes.append({static_cast<quint32>(m_char.toLatin1()), QString(m_char), m_line, m_col});
               if (m_char == '(') inId = false;
               else if (m_char == ')' && m_nextc == '.') inId = true;
            }

            next_char();
            break;
         }

         case Id        :
         {
            if (m_char.isLetterOrNumber() || m_char == '_') {
               text.append(m_char);
               next_char();
            }
            else {                       // end of id
               if      (text == "true")  m_lexemes.append({ArtiToken::True, text, sline, scol});
               else if (text == "false") m_lexemes.append({ArtiToken::True, text, sline, scol});
               else if (text == "null")  m_lexemes.append({ArtiToken::Null, text, sline, scol});
               else                      m_lexemes.append({ArtiToken::Id, text, sline, scol});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               inId   = (m_char == '.');
               state  = InTagEval;
            }
            break;
         }

         case Sysid     :
         {
            if (m_char.isLetterOrNumber()) {
               text.append(m_char);
               next_char();
            }
            else {                       // end of id
               m_lexemes.append({ArtiToken::Sysid, text, sline, scol});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               state  = InTagEval;
            }
            break;
         }

         case Sysnum    :
         {
            if (m_char.isDigit()) {
               text.append(m_char);
               next_char();
            }
            else {                       // end of id
               m_lexemes.append({ArtiToken::Sysid, text, sline, scol});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               state  = InTagEval;
            }
            break;
         }

         case Hex        :
         {
            QChar c = m_char.toLower();
            if (c.isDigit() || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f') {
               text.append(m_char);
               next_char();
            }
            else if (m_char == '_' && m_nextc != '_')      // skip
               next_char();
            else {            // end of hex number
               m_lexemes.append({ArtiToken::Hex, text, sline, scol});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               state  = InTagEval;
            }
            break;
         }

         case Dec       :
         {
            if (m_char.isDigit()) {
               text.append(m_char);
               next_char();
            }
            else if (m_char == '.' && !inId) {
               text.append(m_char);
               state = Fraction;
               next_char();
            }
            else {         // end of dec number
               m_lexemes.append({ArtiToken::Dec, text, sline, scol});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               state  = InTagEval;
            }
            break;
         }

         case Oct       :
         {
            if (m_char.isDigit() && m_char != '8' && m_char != '9') {
               text.append(m_char);
               next_char();
            }
            else if (m_char == '_' && m_nextc != '_')      // skip
               next_char();
            else {            // end of oct number
               m_lexemes.append({ArtiToken::Oct, text, sline, scol});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               state  = InTagEval;
            }
            break;
         }

         case Bin       :
         {
            if (m_char == '0' || m_char == '1') {
               text.append(m_char);
               next_char();
            }
            else if (m_char == '_' && m_nextc != '_')      // skip
               next_char();
            else {            // end of bin number
               m_lexemes.append({ArtiToken::Bin, text, sline, scol});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               state  = InTagEval;
            }
            break;
         }

         case Fraction  :
         {
            if (m_char.isDigit()) {
               text.append(m_char);
               next_char();
            }
            else if (m_char.toLower() == 'e') {
               text.append(m_char);
               next_char();
               if (m_char == '-' || m_char == '+') {
                  text.append(m_char);
                  next_char();
               }
               if (m_char.isDigit())
                  state = Exponent;
               else {
                  syntax_error({"<digit>"});
                  sline  = m_line;
                  scol   = m_col;
                  state  = InTagEval;
               }
            }
            else {            // end of fraction
               m_lexemes.append({ArtiToken::Real, text, sline, scol});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               state  = InTagEval;
            }
            break;
         }

         case Exponent  :
         {
            if (m_char.isDigit()) {
               text.append(m_char);
               next_char();
            }
            else {
               m_lexemes.append({ArtiToken::Real, text, sline, scol});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               state  = InTagEval;
            }
            break;
         }

         case Sstring   :
         {
            if (m_char == '\'') {
               m_lexemes.append({ArtiToken::SString, text, sline, scol});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               state  = InTagEval;
            }
            else text.append(m_char);

            next_char();
            break;
         }

         case Dstring   :
         {
            if (m_char == '"') {
               m_lexemes.append({ArtiToken::DString, text, sline, scol});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               state  = InTagEval;
            }
            else text.append(m_char);

            next_char();
            break;
         }

         case Regex   :
         {
            if (m_char == '/' && m_prevc != '\\') {
               m_lexemes.append({ArtiToken::Regex, text, sline, scol});
               text.clear();
               sline  = m_line;
               scol   = m_col;
               state  = InTagEval;
            }
            else text.append(m_char);

            next_char();
            break;
         }

         case Comment   :
         {
            if (m_char == '#' && m_prevc != '\\' && m_nextc == '}') {   // end of comment
               next_char();      // read past '}'
               sline  = m_line;
               scol   = m_col;
               eot    = true;
               state  = Text;
            }
            next_char();
            break;
         }
      }
   }
   if (!text.isEmpty())
      m_lexemes.append({ArtiToken::Text, text, sline, scol});

   m_lexemes.append({ArtiToken::End, QString(), m_line, m_col});
}

void Parser::next_char ()
{
   m_prevc = m_char;

   if (m_char.isNull()) {      // starting
      if (!m_stream.atEnd()) m_stream >> m_char;
      if (!m_stream.atEnd()) m_stream >> m_nextc;
   }
   else {
      m_char = m_nextc;
      if (!m_stream.atEnd()) m_stream >> m_nextc;
      else m_nextc = QChar();  // null char
   }

   if (m_char == '\n') {
      m_line++;
       m_col = 1;
   }
   else
       m_col++;
}
void Parser::skip_ws () {while (m_char.isSpace()) next_char();}
QString Parser::trim_ws (QString text)
{
   static const QRegularExpression re("\\s+$");
   return text.remove(re);
}

//-----------------------------------------------------------------------------
// utility functions
//-----------------------------------------------------------------------------
QString Parser::token_name (quint32 token) const
{
   if (token < 128) return QString(QChar(token));
   return TokenName(token);
}

void Parser::syntax_error (QStringList exp)
{
   if (!m_status) return;

   m_artiData->errors++;

   QString expected;
   if (exp.size())
      expected = QString("Expected: %1").arg(exp.join(", "));

   MESSAGE(SysError, tr("Unexpected token '%1' (%2). %3").arg(token_name(next_lex().token)).arg(next_lex().token).arg(expected), m_artiData->filename, next_lex().line, next_lex().col);
   m_status = false;
}

void Parser::dump_lex ()
{
   foreach (auto lex, m_lexemes)
      MESSAGE(Info, QString("%1 [%2 : %3] - %4").arg(token_name(lex.token)).arg(lex.line).arg(lex.col).arg(lex.value));
}
