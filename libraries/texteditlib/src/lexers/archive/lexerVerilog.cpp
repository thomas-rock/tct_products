#include "lexerVerilog.h"

using namespace editorLib;

lexerVerilog::lexerVerilog(QObject* parent)
   : QsciLexerVerilog(parent)
{
}

lexerVerilog::~lexerVerilog()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// language
const char* lexerVerilog::language() const
{
   return "Verilog";
}

// defaultFont
QFont lexerVerilog::defaultFont (int style) const
{
   QFont f = QsciLexerVerilog::defaultFont(style);
   f.setFamily("Courier New");
   f.setPointSize(10);
   return f;
}
