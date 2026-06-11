#pragma once

#include <Qsci/qscilexer.h>

namespace editorLib
{
   typedef QMap<QString, QsciLexer*>   lexerMap;
   class lexerFactory
   {
      private:
         enum LexerTypes
         {
            UNKNOWN  = 0,
            ARTI     = 1,
            BNF,
            CPP,
            HTML,
            INI,
            JSON,
            PERL,
            PYTHON,
            RUBY,
            SYSVLOG,
            TCL,
            TEXT,
            VHDL,
            VLOG,
            XML
         };

         static QMap<QString, LexerTypes> _ext2Type;
         static QMap<QString, QString>    _ext2Name;

      public:
                                    lexerFactory ();

         static QsciLexer*          lexer (QString ext);
         static QsciLexer*          lexerByName (QString name);
         static lexerMap            allLexers ();

         static QString             ext2Name (QString ext);
         static QString             fileFilters ();
         static QStringList         lexerNames ();

      protected:
         static QsciLexer*          createLexer (LexerTypes type);
   };
}
