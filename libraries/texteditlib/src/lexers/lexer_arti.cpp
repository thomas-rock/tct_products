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
#include "lexer_arti.h"
#include <QRegularExpression>
#include <QSettings>
#include <Qsci/qsciscintilla.h>

#include <QDebug>

using namespace texteditLib;

enum Styles
{
   Default     = 0,
   Tag,
   Eval,
   Keyword,
   Function,
   Number,
   String,
   Comment
};

enum States
{
   sText,
   sTag,
   sEval,
   sComment
};


LexerArti::LexerArti (QObject* parent) : QsciLexerCustom(parent) {}
LexerArti::~LexerArti () {}

//-----------------------------------------------------------------------------
const char* LexerArti::language () const {return "Arti";}
QString LexerArti::description (int style) const
{
   switch(style)
   {
      case Default   : return "Default";
      case Tag       : return "Tag";
      case Eval      : return "Eval";
      case Keyword   : return "Keyword";
      case Function  : return "Function";
      case Number    : return "Number";
      case String    : return "String";
      case Comment   : return "Comment";
    }
    return QString();
}

//-----------------------------------------------------------------------------
void LexerArti::styleText (int start, int end)
{
   QsciScintilla* editor = QsciLexer::editor();
   if(editor == nullptr)
      return;

   m_source = editor->text();

//   char* data = new char[end - start + 1];
//   editor->SendScintilla(QsciScintilla::SCI_GETTEXTRANGE, start, end, data);
//   _source = QString(data);
//   delete [] data;
   if(m_source.isEmpty())
      return;

   m_source.append(QChar('\0'));

   // make everything default style
//   int offset = (end == editor->length()) ? 0 : 1;
   int length = editor->length();
   startStyling(0);
   setStyling(length-1, Default);

   // get initial state
//   int line;
//   int pos;
//   editor->lineIndexFromPosition(start, &line, &pos);
//   int state = _endState.value(line-1, sText);
   int state = sText;

//qDebug() << QString("Styling: %1").arg(_source);
//qDebug() << QString("current line: %1, previous state: %2").arg(line).arg((state == sText) ? "Text" : (state == sTag) ? "Tag" : (state == sEval) ? "Eval" : "Comment");

   int style_start = 0;
   m_text.clear();
   for (int index = 0; index < length; index++) {
      QChar c = m_source[index];
      QChar n = m_source[index+1];

      if (state == sText) {
         if (c == '{' && n == '%') {
            m_text = c;
            state = sTag;
            start = index;
         }
         else if (c == '{' && n == '{') {
            m_text = c;
            state = sEval;
            start = index;
         }
         else if (c == '{' && n == '#') {
            state = sComment;
            start = index;
         }
      }
      else if (state == sTag) {
         m_text.append(c);
         if (c == '%' && n == '}') {     // find end of tag
            m_text.append(n);
            style(m_functions, start, Function);    // style functions before other things otherwise it thinks if (...) is a function
            style(m_tag,       start, Tag);
            style(m_eval,      start, Eval);
            style(m_keywords,  start, Keyword);
            style(m_numbers,   start, Number);
            style(m_strings,   start, String);
            state = sText;
         }
      }
      else if (state == sEval) {
         m_text.append(c);
         if (c == '}' && n == '}') {     // find end of tag
            m_text.append(c);
            style(m_functions, start, Function);    // style functions before other things otherwise it thinks if (...) is a function
            style(m_tag,       start, Tag);
            style(m_eval,      start, Eval);
            style(m_keywords,  start, Keyword);
            style(m_numbers,   start, Number);
            style(m_strings,   start, String);
            state = sText;
         }
      }
      else if (state == sComment) {
         if (c == '#' && n == '}') {     // find end of tag
            startStyling(start);
            setStyling(index-start+2, Comment);
            state = sText;
         }
      }
   }
}

//-----------------------------------------------------------------------------
// defaultColor
QColor LexerArti::defaultColor (int style) const
{
   switch(style) {
      case Tag       : return Qt::darkRed;
      case Eval      : return Qt::darkRed;
      case Keyword   : return Qt::blue;
      case Function  : return Qt::darkYellow;
      case Number    : return QColor("#ff8000");
      case String    : return QColor("#0080C0");
      case Comment   : return Qt::darkGreen;
    }

    return Qt::black;
}

// defaultFont
QFont LexerArti::defaultFont (int style) const
{
   QFont font("Source Code Pro", 11);
   if      (style == Comment) font.setItalic(true);
//   else if (style == Tag) font.setBold(true);

   return font;
}

//-----------------------------------------------------------------------------
void LexerArti::style (QList<QRegularExpression*> patterns, int start, int style)
{
   foreach (QRegularExpression* re, patterns)
   {
      QRegularExpressionMatchIterator i = re->globalMatch(m_text);
      while (i.hasNext())
      {
         QRegularExpressionMatch match = i.next();
         startStyling(start + match.capturedStart());
         setStyling(match.capturedLength(), style);
      }
   }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QList<QRegularExpression*> LexerArti::m_tag
{
   new QRegularExpression("\\{\\%"),
   new QRegularExpression("\\%\\}")
};

QList<QRegularExpression*> LexerArti::m_eval
{
   new QRegularExpression("\\{\\{"),
   new QRegularExpression("\\}\\}")
};

QList<QRegularExpression*> LexerArti::m_keywords
{
   new QRegularExpression("\\balign\\b"),
   new QRegularExpression("\\belse\\b"),
   new QRegularExpression("\\belsif\\b"),
   new QRegularExpression("\\bendfor\\b"),
   new QRegularExpression("\\bendif\\b"),
   new QRegularExpression("\\bfor\\b"),
   new QRegularExpression("\\bif\\b"),
   new QRegularExpression("\\bin\\b"),
   new QRegularExpression("\\binclude\\b"),
   new QRegularExpression("\\bload\\b"),
   new QRegularExpression("\\bmessage\\b"),
   new QRegularExpression("\\bset\\b"),
   new QRegularExpression("\\btemplate\\b"),
   new QRegularExpression("\\bwith\\b")
};

QList<QRegularExpression*> LexerArti::m_functions
{
   new QRegularExpression("\\b[_A-Za-z]\\w*(?=\\s*\\()")
};

QList<QRegularExpression*> LexerArti::m_numbers
{
   new QRegularExpression("\\b0b[01_]+\\b"),
   new QRegularExpression("\\b0o[0-7_]+\\b"),
   new QRegularExpression("\\b0x[0-9A-Fa-f_]+\\b"),
   new QRegularExpression("(?<!\\$)\\b[0-9]+\\b"),
   new QRegularExpression("\\b[0-9]*\\.[0-9]+([eE] [-+]? [0-9]+)?\\b")
};

QList<QRegularExpression*> LexerArti::m_strings
{
   new QRegularExpression("(?<!\')\".*\"", QRegularExpression::InvertedGreedinessOption),
   new QRegularExpression("(?<!\")\'.*\'", QRegularExpression::InvertedGreedinessOption)
};

QList<QRegularExpression*> LexerArti::m_comments
{
   new QRegularExpression("{#.*#}", QRegularExpression::InvertedGreedinessOption)
};
