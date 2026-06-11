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
#include "lexer_text.h"
#include <QRegularExpression>

using namespace texteditLib;

enum Styles
{
   Default     = 0
};

LexerText::LexerText(QObject* parent)
   : QsciLexerCustom(parent)
{
}

LexerText::~LexerText()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// language
const char* LexerText::language() const
{
   return "Text";
}

// description
QString LexerText::description (int style) const
{
   switch(style)
   {
      case Default   : return "Default";
   }
   return QString();
}

//-----------------------------------------------------------------------------
// styleText
void LexerText::styleText(int start, int end)
{
   Q_UNUSED(start)
   Q_UNUSED(end)
}

//-----------------------------------------------------------------------------
// defaultColor
QColor LexerText::defaultColor (int style) const
{
   Q_UNUSED(style)

   return Qt::black;
}

// defaultFont
QFont LexerText::defaultFont (int style) const
{
   Q_UNUSED(style)

   QFont font("Courier New", 12);
   return font;
}

//--------------------
// protected functions
//--------------------
//-----------------------------------------------------------------------------
// style
void LexerText::style(QList<QRegularExpression*> patterns, int start, int style)
{
   Q_UNUSED(patterns)
   Q_UNUSED(start)
   Q_UNUSED(style)
}

