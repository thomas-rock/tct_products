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
#include "Lexer_ini.h"
#include <QRegularExpression>
#include <Qsci/qsciscintilla.h>

#include <QDebug>

using namespace texteditLib;

enum Styles
{
   Default     = 0,
   Group,
   Key,
   Comment
};

LexerINI::LexerINI(QObject* parent)
   : QsciLexerCustom(parent)
{
}

LexerINI::~LexerINI()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// language
const char* LexerINI::language() const
{
   return "INI";
}

// description
QString LexerINI::description (int style) const
{
   switch(style)
   {
      case Default   : return "Default";
      case Group     : return "Group";
      case Key       : return "Key";
      case Comment   : return "Comment";
    }
    return QString();
}

//-----------------------------------------------------------------------------
// styleText
void LexerINI::styleText(int start, int end)
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
   style(m_groups, start, Group);
   style(m_keys, start, Key);
   style(m_comments, start, Comment);

   // folding
   // goes here....
}

//-----------------------------------------------------------------------------
// defaultColor
QColor LexerINI::defaultColor (int style) const
{
   switch(style) {
      case Group     : return Qt::blue;
      case Key       : return Qt::darkMagenta;
      case Comment   : return Qt::darkGreen;
    }

    return Qt::black;
}

// defaultFont
QFont LexerINI::defaultFont (int style) const
{
   QFont font("Courier New", 10);
   if (style == Group)
      font.setBold(true);
   else if (style == Comment)
      font.setItalic(true);
   return font;
}

//--------------------
// protected functions
//--------------------
//-----------------------------------------------------------------------------
// style
void LexerINI::style(QList<QRegularExpression*> patterns, int start, int style)
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
QList<QRegularExpression*> LexerINI::m_groups
{
   new QRegularExpression("(^|\\n)\\s*\\[[\\w/]+\\]")
};

QList<QRegularExpression*> LexerINI::m_keys
{
   new QRegularExpression("(^|\\n)\\s*\\w+\\s*(?=\\=)")
};

QList<QRegularExpression*> LexerINI::m_comments
{
   new QRegularExpression("(^|\\n)\\s*;[^\\n]*")
};


