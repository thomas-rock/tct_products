#include "lexerVHDL.h"

using namespace editorLib;

lexerVHDL::lexerVHDL(QObject* parent)
   : QsciLexerVHDL(parent)
{
}

lexerVHDL::~lexerVHDL()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// language
const char* lexerVHDL::language() const
{
   return "VHDL";
}

// defaultFont
QFont lexerVHDL::defaultFont (int style) const
{
   QFont f = QsciLexerVHDL::defaultFont(style);
   f.setFamily("Courier New");
   f.setPointSize(10);
   return f;
}
