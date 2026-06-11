#include "lexerFactory.h"

#include "lexerArti.h"
#include "lexerBNF.h"
#include "lexerCPP.h"
#include "lexerHTML.h"
#include "lexerINI.h"
#include "lexerJson.h"
#include "lexerPerl.h"
#include "lexerPython.h"
#include "lexerRuby.h"
#include "lexerTCL.h"
#include "lexerText.h"
#include "lexerVerilog.h"
#include "lexerVHDL.h"
#include "lexerXML.h"

using namespace editorLib;

lexerFactory::lexerFactory ()
{
}

//-----------------
// public functions
//-----------------
//-----------------------------------------------------------------------------
// lexer
QsciLexer* lexerFactory::lexer (QString ext)
{
   return createLexer(_ext2Type.value(ext.toLower(), UNKNOWN));
}

// lexer
QsciLexer* lexerFactory::lexerByName (QString name)
{
   name = name.toLower();
   if      (name == "arti")            return createLexer(lexerFactory::ARTI);
   else if (name == "bnf")             return createLexer(lexerFactory::BNF);
   else if (name == "cpp")             return createLexer(lexerFactory::CPP);
   else if (name == "c/c++")           return createLexer(lexerFactory::CPP);
   else if (name == "html")            return createLexer(lexerFactory::HTML);
   else if (name == "ini")             return createLexer(lexerFactory::INI);
   else if (name == "json")            return createLexer(lexerFactory::JSON);
   else if (name == "perl")            return createLexer(lexerFactory::PERL);
   else if (name == "python")          return createLexer(lexerFactory::PYTHON);
   else if (name == "ruby")            return createLexer(lexerFactory::RUBY);
   else if (name == "tcl")             return createLexer(lexerFactory::TCL);
   else if (name == "text")            return createLexer(lexerFactory::TEXT);
   else if (name == "verilog")         return createLexer(lexerFactory::VLOG);
   else if (name == "vhdl")            return createLexer(lexerFactory::VHDL);
   else if (name == "xml")             return createLexer(lexerFactory::XML);
   else                                return nullptr;
}

// allLexers
lexerMap lexerFactory::allLexers()
{
   lexerMap rtn;

//   QSettings settings;

//   QsciLexer* lexer = new xLexerCPP;
//   lexer->readSettings(settings);
//   rtn.insert(lexer->language(), lexer);

//   lexer = new xLexerDGL;
//   lexer->readSettings(settings);
//   rtn.insert(lexer->language(), lexer);

//   lexer = new xLexerHTML;
//   lexer->readSettings(settings);
//   rtn.insert(lexer->language(), lexer);

//   lexer = new xLexerINI;
//   lexer->readSettings(settings);
//   rtn.insert(lexer->language(), lexer);

//   lexer = new xLexerJson;
//   lexer->readSettings(settings);
//   rtn.insert(lexer->language(), lexer);

//   lexer = new xLexerPerl;
//   lexer->readSettings(settings);
//   rtn.insert(lexer->language(), lexer);

//   lexer = new xLexerPython;
//   lexer->readSettings(settings);
//   rtn.insert(lexer->language(), lexer);

//   lexer = new xLexerRuby;
//   lexer->readSettings(settings);
//   rtn.insert(lexer->language(), lexer);

//   lexer = new xLexerTCL;
//   lexer->readSettings(settings);
//   rtn.insert(lexer->language(), lexer);

//   lexer = new xLexerVHDL;
//   lexer->readSettings(settings);
//   rtn.insert(lexer->language(), lexer);

//   lexer = new xLexerVerilog;
//   lexer->readSettings(settings);
//   rtn.insert(lexer->language(), lexer);

//   lexer = new xLexerXML;
//   lexer->readSettings(settings);
//   rtn.insert(lexer->language(), lexer);

   return rtn;
}

//-----------------------------------------------------------------------------
// ext2Name
QString lexerFactory::ext2Name (QString ext)
{
   return _ext2Name.value(ext, "All Files");
}

// fileFilters
QString lexerFactory::fileFilters ()
{
   return   "Arti (*,arti);;"
            "BNF (*.y);;"
            "C/C++ (*.cpp, *.c *.hpp *.h);;"
            "HTML (*.htm *.html);;"
            "INI (*.ini);;"
            "Json++ (*.jpp);;"
            "Json (*.json);;"
            "Perl (*.pl);;"
            "Python (*.py);;"
            "Ruby (*.rb);;"
            "SystemVerilog (*.sv *.svh);;"
            "TCL (*.tcl);;"
            "Text (*.txt);;"
            "Verilog (*.v *.vh *.vl);;"
            "VHDL (*.vhd *.vhdl);;"
            "XML (*.xml);;"
            "All Files (*.*)";
}

// lexerNames
QStringList lexerFactory::lexerNames ()
{
   QStringList names = _ext2Name.values();
   names.removeDuplicates();
   return names;
}

//--------------------
// protected functions
//--------------------
//-----------------------------------------------------------------------------
// createLexer
QsciLexer* lexerFactory::createLexer (LexerTypes type)
{
   if      (type == lexerFactory::ARTI)      return new lexerArti;
   else if (type == lexerFactory::BNF)       return new lexerBNF;
   else if (type == lexerFactory::CPP)       return new lexerCPP;
   else if (type == lexerFactory::HTML)      return new lexerHTML;
   else if (type == lexerFactory::INI)       return new lexerINI;
   else if (type == lexerFactory::JSON)      return new lexerJson;
   else if (type == lexerFactory::PERL)      return new lexerPerl;
   else if (type == lexerFactory::PYTHON)    return new lexerPython;
   else if (type == lexerFactory::RUBY)      return new lexerRuby;
   else if (type == lexerFactory::TCL)       return new lexerTCL;
   else if (type == lexerFactory::TEXT)      return new lexerText;
   else if (type == lexerFactory::VHDL)      return new lexerVHDL;
   else if (type == lexerFactory::VLOG)      return new lexerVerilog;
   else if (type == lexerFactory::XML)       return new lexerXML;

   return new lexerText;
}

//------------------
// private functions
//------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QMap<QString, lexerFactory::LexerTypes> lexerFactory::_ext2Type
{
   {"arti",    lexerFactory::ARTI},
   {"cpp",     lexerFactory::CPP},
   {"c",       lexerFactory::CPP},
   {"h",       lexerFactory::CPP},
   {"hpp",     lexerFactory::CPP},
   {"htm",     lexerFactory::HTML},
   {"html",    lexerFactory::HTML},
   {"ini",     lexerFactory::INI},
   {"json",    lexerFactory::JSON},
   {"pl",      lexerFactory::PERL},
   {"py",      lexerFactory::PYTHON},
   {"rb",      lexerFactory::RUBY},
   {"sv",      lexerFactory::VLOG},
   {"svh",     lexerFactory::VLOG},
   {"tcl",     lexerFactory::TCL},
   {"txt",     lexerFactory::TEXT},
   {"vhd",     lexerFactory::VHDL},
   {"vhdl",    lexerFactory::VHDL},
   {"v",       lexerFactory::VLOG},
   {"vh",      lexerFactory::VLOG},
   {"vl",      lexerFactory::VLOG},
   {"xml",     lexerFactory::XML},
   {"y",       lexerFactory::BNF}
};

QMap<QString, QString> lexerFactory::_ext2Name
{
   {"arti",    "Arti"},
   {"cpp",     "C/C++"},
   {"c",       "C/C++"},
   {"h",       "C/C++"},
   {"hpp",     "C/C++"},
   {"htm",     "HTML"},
   {"html",    "HTML"},
   {"ini",     "INI"},
   {"json",    "Json"},
   {"pl",      "Perl"},
   {"py",      "Python"},
   {"rb",      "Ruby"},
   {"sv",      "SystemVerilog"},
   {"svh",     "SystemVerilog"},
   {"tcl",     "TCL"},
   {"txt",     "Text"},
   {"vhd",     "VHDL"},
   {"vhdl",    "VHDL"},
   {"v",       "Verilog"},
   {"vh",      "Verilog"},
   {"vl",      "Verilog"},
   {"xml",     "XML"},
   {"y",       "BNF"}
};
