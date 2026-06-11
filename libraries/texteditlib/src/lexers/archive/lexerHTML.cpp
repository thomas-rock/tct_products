#include "lexerHTML.h"

using namespace editorLib;


lexerHTML::lexerHTML(QObject* parent)
   : QsciLexerHTML(parent)
{
}

lexerHTML::~lexerHTML()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// language
const char* lexerHTML::language() const
{
   return "HTML";
}

// defaultFont
QFont lexerHTML::defaultFont (int style) const
{
   QFont f = QsciLexerHTML::defaultFont(style);
   f.setFamily("Courier New");
   f.setPointSize(10);
   return f;
}
