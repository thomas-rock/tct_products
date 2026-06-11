#pragma once

#include<Qsci/qscilexerruby.h>

namespace editorLib
{
   class lexerRuby : public QsciLexerRuby
   {
      Q_OBJECT

      public:
                              lexerRuby (QObject* parent = nullptr);
                              ~lexerRuby () override;

         const char*          language() const override;
         QFont                defaultFont (int style) const override;
   };
}
