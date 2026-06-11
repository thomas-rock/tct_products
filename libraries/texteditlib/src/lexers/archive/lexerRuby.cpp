#include "lexerRuby.h"

using namespace editorLib;

lexerRuby::lexerRuby(QObject* parent)
   : QsciLexerRuby(parent)
{
}

lexerRuby::~lexerRuby()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// language
const char* lexerRuby::language() const
{
   return "Ruby";
}

// defaultFont
QFont lexerRuby::defaultFont (int style) const
{
   QFont f = QsciLexerRuby::defaultFont(style);
   f.setFamily("Courier New");
   f.setPointSize(10);
   return f;
}
