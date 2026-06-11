#pragma once

#include<Qsci/qscilexerperl.h>

namespace editorLib
{
   class lexerPerl : public QsciLexerPerl
   {
      Q_OBJECT

      public:
                              lexerPerl (QObject* parent = nullptr);
                              ~lexerPerl () override;

         const char*          language() const override;
         QFont                defaultFont (int style) const override;
   };
}
