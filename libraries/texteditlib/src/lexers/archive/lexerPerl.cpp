#include "lexerPerl.h"

using namespace editorLib;

lexerPerl::lexerPerl(QObject* parent)
   : QsciLexerPerl(parent)
{
}

lexerPerl::~lexerPerl()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// language
const char* lexerPerl::language() const
{
   return "Perl";
}

// defaultFont
QFont lexerPerl::defaultFont (int style) const
{
   QFont f = QsciLexerPerl::defaultFont(style);
   f.setFamily("Courier New");
   f.setPointSize(10);
   return f;
}
