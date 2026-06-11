#include "lexerPython.h"

using namespace editorLib;

lexerPython::lexerPython(QObject* parent)
   : QsciLexerPython(parent)
{
}

lexerPython::~lexerPython()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// language
const char* lexerPython::language() const
{
   return "Python";
}

// defaultFont
QFont lexerPython::defaultFont (int style) const
{
   QFont f = QsciLexerPython::defaultFont(style);
   f.setFamily("Courier New");
   f.setPointSize(10);
   return f;
}
