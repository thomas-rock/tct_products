//=============================================================================
// Treasure Coast Technologies, Inc.          www.TreasureCoastTechnologies.com
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from RockSolid Solutions.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2026 to the present, Treasure Coast Technologies, Inc.
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
//
// Description :
//
//=============================================================================
#include "lexer_manager.h"
#include "editor_settings_p.h"

#include "lexers/lexer_arti.h"
#include "lexers/lexer_bnf.h"
#include "lexers/lexer_ini.h"
#include "lexers/lexer_text.h"

#include "Qsci/qscilexerbash.h"
#include "Qsci/qscilexerbatch.h"
#include "Qsci/qscilexercoffeescript.h"
#include "Qsci/qscilexercpp.h"
#include "Qsci/qscilexercsharp.h"
#include "Qsci/qscilexercss.h"
#include "Qsci/qscilexerd.h"
#include "Qsci/qscilexerdiff.h"
#include "Qsci/qscilexerfortran.h"
#include "Qsci/qscilexerfortran77.h"
#include "Qsci/qscilexerhtml.h"
#include "Qsci/qscilexeridl.h"
#include "Qsci/qscilexerjava.h"
#include "Qsci/qscilexerjavascript.h"
#include "Qsci/qscilexerjson.h"
#include "Qsci/qscilexerlua.h"
#include "Qsci/qscilexermakefile.h"
#include "Qsci/qscilexermarkdown.h"
#include "Qsci/qscilexermatlab.h"
#include "Qsci/qscilexeroctave.h"
#include "Qsci/qscilexerpascal.h"
#include "Qsci/qscilexerperl.h"
#include "Qsci/qscilexerpostscript.h"
#include "Qsci/qscilexerpov.h"
#include "Qsci/qscilexerpython.h"
#include "Qsci/qscilexerruby.h"
#include "Qsci/qscilexersql.h"
#include "Qsci/qscilexertcl.h"
#include "Qsci/qscilexerverilog.h"
#include "Qsci/qscilexervhdl.h"
#include "Qsci/qscilexerxml.h"
#include "Qsci/qscilexeryaml.h"

#include <QSettings>

using namespace texteditLib;

//-----------------------------------------------------------------------------
// static storage
//-----------------------------------------------------------------------------
const QMap<LexerManager::LexerTypes, QStringList> LexerManager::m_lexer_info = {
   {LexerManager::ARTI,       {"arti"}},
   {LexerManager::BASH,       {}},
   {LexerManager::BATCH,      {}},
   {LexerManager::BNF,        {"bnf"}},
   {LexerManager::COFFEE,     {}},
   {LexerManager::CPP,        {"c", "cpp", "c++", "h", "hpp"}},
   {LexerManager::CSHARP,     {}},
   {LexerManager::CSS,        {}},
   {LexerManager::D,          {}},
   {LexerManager::DIFF,       {}},
   {LexerManager::FORTRAN,    {}},
   {LexerManager::FORTRAN77,  {}},
   {LexerManager::HTML,       {"html", "htm"}},
   {LexerManager::IDL,        {}},
   {LexerManager::INI,        {"ini"}},
   {LexerManager::JAVA,       {}},
   {LexerManager::JAVASCRIPT, {}},
   {LexerManager::JSON,       {"json", "json5"}},
   {LexerManager::LUA,        {}},
   {LexerManager::MAKEFILE,   {}},
   {LexerManager::MARKDOWN,   {}},
   {LexerManager::MATLAB,     {}},
   {LexerManager::OCTAVE,     {}},
   {LexerManager::PAS,        {}},
   {LexerManager::PERL,       {"pl"}},
   {LexerManager::POSTSCRIPT, {"ps"}},
   {LexerManager::POV,        {}},
   {LexerManager::PYTHON,     {"py"}},
   {LexerManager::RUBY,       {"rb"}},
   {LexerManager::SQL,        {}},
   {LexerManager::TCL,        {"tcl"}},
   {LexerManager::TEXT,       {}},
   {LexerManager::VHDL,       {"vhdl", "vhd"}},
   {LexerManager::VLOG,       {"v", "vl", "sv", "svh"}},
   {LexerManager::XML,        {"xml"}},
   {LexerManager::YAML,       {"yaml"}}
};
QMap<LexerManager::LexerTypes, QStringList>  LexerManager::m_user_extensions;
QMap<LexerManager::LexerTypes, QString>      LexerManager::m_languages;
QStringList                                  LexerManager::m_file_filters;

LexerManager::LexerManager () {}

//-----------------------------------------------------------------------------
void LexerManager::initialize ()
{
   m_file_filters.clear();

   foreach (LexerManager::LexerTypes type, m_lexer_info.keys()) {
      QsciLexer* lexer = createLexer(type);
      m_languages.insert(type, lexer->language());
      m_file_filters.append(QString("%1 (*.%2)").arg(lexer->language()).arg(m_lexer_info[type].join(" *.")));
   }
}
//-----------------------------------------------------------------------------
QStringList LexerManager::languages () {return m_languages.values();}
QString LexerManager::fileFilters () {return QString("%1;;All Files (*.*)").arg(m_file_filters.join(";;"));}
QsciLexer* LexerManager::lexerByExt (const QString& ext)
{
   QString ext_lc = ext.toLower();
   foreach (LexerManager::LexerTypes type, m_lexer_info.keys()) {
      if (m_lexer_info[type].contains(ext_lc))
         return createLexer(type);
   }
   return createLexer(LexerManager::TEXT);
}
QsciLexer* LexerManager::lexerByLang (const QString& lang)
{
   foreach (LexerManager::LexerTypes type, m_lexer_info.keys()) {
      if (m_languages.value(type) == lang)
         return createLexer(type);
   }
   return createLexer(LexerManager::TEXT);
}

lexerMap LexerManager::lexers ()
{
   lexerMap rtn;

   foreach (LexerManager::LexerTypes type, m_lexer_info.keys()) {
      QsciLexer* lexer = createLexer(type);
      rtn.insert(lexer->language(), lexer);
   }
   return rtn;
}
QsciLexer* LexerManager::createLexer (LexerTypes type)
{
   QsciLexer* lexer;
   switch (type) {
      case LexerManager::ARTI       : lexer = new LexerArti;               break;
      case LexerManager::BASH       : lexer = new QsciLexerBash;           break;
      case LexerManager::BATCH      : lexer = new QsciLexerBatch;          break;
      case LexerManager::BNF        : lexer = new LexerBNF;                break;
      case LexerManager::COFFEE     : lexer = new QsciLexerCoffeeScript;   break;
      case LexerManager::CPP        : lexer = new QsciLexerCPP;            break;
      case LexerManager::CSHARP     : lexer = new QsciLexerCSharp;         break;
      case LexerManager::CSS        : lexer = new QsciLexerCSS;            break;
      case LexerManager::D          : lexer = new QsciLexerD;              break;
      case LexerManager::DIFF       : lexer = new QsciLexerDiff;           break;
      case LexerManager::FORTRAN    : lexer = new QsciLexerFortran;        break;
      case LexerManager::FORTRAN77  : lexer = new QsciLexerFortran77;      break;
      case LexerManager::HTML       : lexer = new QsciLexerHTML;           break;
      case LexerManager::IDL        : lexer = new QsciLexerIDL;            break;
      case LexerManager::INI        : lexer = new LexerINI;                break;
      case LexerManager::JAVA       : lexer = new QsciLexerJava;           break;
      case LexerManager::JAVASCRIPT : lexer = new QsciLexerJavaScript;     break;
      case LexerManager::JSON       : lexer = new QsciLexerJSON;           break;
      case LexerManager::LUA        : lexer = new QsciLexerLua;            break;
      case LexerManager::MAKEFILE   : lexer = new QsciLexerMakefile;       break;
      case LexerManager::MARKDOWN   : lexer = new QsciLexerMarkdown;       break;
      case LexerManager::MATLAB     : lexer = new QsciLexerMatlab;         break;
      case LexerManager::OCTAVE     : lexer = new QsciLexerOctave;         break;
      case LexerManager::PAS        : lexer = new QsciLexerPascal;         break;
      case LexerManager::PERL       : lexer = new QsciLexerPerl;           break;
      case LexerManager::POSTSCRIPT : lexer = new QsciLexerPostScript;     break;
      case LexerManager::POV        : lexer = new QsciLexerPOV;            break;
      case LexerManager::PYTHON     : lexer = new QsciLexerPython;         break;
      case LexerManager::RUBY       : lexer = new QsciLexerRuby;           break;
      case LexerManager::SQL        : lexer = new QsciLexerSQL;            break;
      case LexerManager::TCL        : lexer = new QsciLexerTCL;            break;
      case LexerManager::VLOG       : lexer = new QsciLexerVerilog;        break;
      case LexerManager::VHDL       : lexer = new QsciLexerVHDL;           break;
      case LexerManager::XML        : lexer = new QsciLexerXML;            break;
      case LexerManager::YAML       : lexer = new QsciLexerYAML;           break;
      default                       : lexer = new LexerText;
   }

   QSettings settings;
   settings.beginGroup(STYLE_SETTINGS);
   lexer->readSettings(settings);

   return lexer;
}
