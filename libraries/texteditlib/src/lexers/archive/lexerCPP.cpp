#include "lexerCPP.h"

using namespace editorLib;


lexerCPP::lexerCPP(QObject* parent)
   : QsciLexerCPP(parent)
{
}

lexerCPP::~lexerCPP()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// language
const char* lexerCPP::language() const
{
   return "C/C++";
}

// defaultFont
QFont lexerCPP::defaultFont (int style) const
{
   QFont f = QsciLexerCPP::defaultFont(style);
   f.setFamily("Courier New");
   f.setPointSize(10);
   return f;
}
