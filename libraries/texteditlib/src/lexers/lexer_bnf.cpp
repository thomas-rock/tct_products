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
#include "Lexer_bnf.h"
#include <QRegularExpression>
#include <Qsci/qsciscintilla.h>

#include <QDebug>

using namespace texteditLib;

enum Styles
{
   Default     = 0,
   Keyword,
   Sysword,
   Name,
   String,
   Comment
};

LexerBNF::LexerBNF(QObject* parent)
   : QsciLexerCustom(parent)
{
}

LexerBNF::~LexerBNF()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// LexerBNF
const char* LexerBNF::language() const
{
   return "BNF";
}

// description
QString LexerBNF::description (int style) const
{
   switch(style)
   {
      case Default   : return "Default";
      case Keyword   : return "Keyword";
      case Sysword   : return "Sysword";
      case Name      : return "Name";
      case String    : return "String";
      case Comment   : return "Comment";
    }
    return QString();
}

//-----------------------------------------------------------------------------
// styleText
void LexerBNF::styleText(int start, int end)
{
   QsciScintilla* editor = QsciLexer::editor();
   if(editor == nullptr)
      return;

   if (end == editor->length()) end--;     // crashes if try to get text at editor()->length() position.

   char* data = new char[end - start + 1];
   editor->SendScintilla(QsciScintilla::SCI_GETTEXTRANGE, start, end, data);
   m_source = QString(data);
   delete [] data;
   if(m_source.isEmpty())
      return;

   // make everything default style
   startStyling(start);
   setStyling(end-start+1, Default);

   // style special things
   style(m_keywords, start, Keyword);
   style(m_syswords, start, Sysword);
   style(m_names, start, Name);
   style(m_strings, start, String);
   style(m_comments, start, Comment);

   // style code

   // style multi-line comments

}

//-----------------------------------------------------------------------------
// defaultColor
QColor LexerBNF::defaultColor (int style) const
{
   switch(style) {
      case Keyword   : return Qt::blue;
      case Sysword   : return Qt::blue;
      case Name      : return Qt::darkMagenta;
      case String    : return QColor("#0080C0");
      case Comment   : return Qt::darkGreen;
    }

    return Qt::black;
}

// defaultFont
QFont LexerBNF::defaultFont (int style) const
{
   QFont font("Source Code Pro", 11);
//   QFont font("Courier New", 10);
   if (style == Comment)
      font.setItalic(true);
   else if (style == Sysword)
      font.setBold(true);

   return font;
}

//--------------------
// protected functions
//--------------------
//-----------------------------------------------------------------------------
// style
void LexerBNF::style(QList<QRegularExpression*> patterns, int start, int style)
{
   foreach (QRegularExpression* re, patterns)
   {
      QRegularExpressionMatchIterator i = re->globalMatch(m_source);
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
QList<QRegularExpression*> LexerBNF::m_keywords
{
   new QRegularExpression("%empty\\b"),
   new QRegularExpression("%left\\b"),
   new QRegularExpression("%none\\b"),
   new QRegularExpression("%right\\b"),
   new QRegularExpression("%whitespace\\b"),
};

QList<QRegularExpression*> LexerBNF::m_syswords
{
   new QRegularExpression("%%"),
};

QList<QRegularExpression*> LexerBNF::m_names
{
   new QRegularExpression("\\s*\\w+(?=[\\n\\s]*\\:)", QRegularExpression::InvertedGreedinessOption)
};

QList<QRegularExpression*> LexerBNF::m_strings
{
   new QRegularExpression("(?<!\")\'.\'", QRegularExpression::InvertedGreedinessOption),
   new QRegularExpression("(?<!\')\".*\"", QRegularExpression::InvertedGreedinessOption)
};

// TODO: Multiline comments via explicit parsing rather than regex
QList<QRegularExpression*> LexerBNF::m_comments
{
   new QRegularExpression("//[^\\n]*")
};

// TODO: Multiline comments via explicit parsing rather than regex
