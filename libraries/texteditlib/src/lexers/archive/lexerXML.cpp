#include "lexerXML.h"

using namespace editorLib;

lexerXML::lexerXML(QObject* parent)
   : QsciLexerXML(parent)
{
}

lexerXML::~lexerXML()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// language
const char* lexerXML::language() const
{
   return "XML";
}

// defaultFont
QFont lexerXML::defaultFont (int style) const
{
   QFont f = QsciLexerXML::defaultFont(style);
   f.setFamily("Courier New");
   f.setPointSize(10);
   return f;
}
