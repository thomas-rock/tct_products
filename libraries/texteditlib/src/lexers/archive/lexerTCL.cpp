#include "lexerTCL.h"

using namespace editorLib;

lexerTCL::lexerTCL(QObject* parent)
   : QsciLexerTCL(parent)
{
}

lexerTCL::~lexerTCL()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// language
const char* lexerTCL::language() const
{
   return "TCL";
}

// defaultFont
QFont lexerTCL::defaultFont (int style) const
{
   QFont f = QsciLexerTCL::defaultFont(style);
   f.setFamily("Courier New");
   f.setPointSize(10);
   return f;
}
