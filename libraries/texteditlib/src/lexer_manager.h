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
#pragma once

#include "Qsci/qscilexer.h"
#include <QString>

namespace texteditLib
{
   typedef QMap<QString, QsciLexer*>   lexerMap;
   class LexerManager
   {
      private:
         enum LexerTypes
         {
            UNKNOWN  = 0,
            ARTI     = 1,
            BASH,
            BATCH,
            BNF,
            COFFEE,
            CPP,
            CSHARP,
            CSS,
            D,
            DIFF,
            FORTRAN,
            FORTRAN77,
            HTML,
            IDL,
            INI,
            JAVA,
            JAVASCRIPT,
            JSON,
            LUA,
            MAKEFILE,
            MARKDOWN,
            MATLAB,
            OCTAVE,
            PAS,        // i.e. PASCAL, which the compiler doesn't like (?)
            PERL,
            POSTSCRIPT,
            POV,
            PYTHON,
            RUBY,
            SQL,
            TCL,
            TEXT,
            VHDL,
            VLOG,
            XML,
            YAML
         };

                                    LexerManager ();

      public:
         static void                initialize ();

         static QStringList         languages ();
         static QString             fileFilters ();
         static QsciLexer*          lexerByExt (const QString& ext);
         static QsciLexer*          lexerByLang (const QString& lang);
         static lexerMap            lexers ();

      protected:
         static QsciLexer*          createLexer (LexerTypes type);

      private:
         static const QMap<LexerTypes, QStringList>   m_lexer_info;
      static QMap<LexerTypes, QStringList>            m_user_extensions;
         static QStringList                           m_file_filters;
         static QMap<LexerTypes, QString>             m_languages;
   };
}
