#pragma once

#include<Qsci/qscilexercpp.h>

namespace editorLib
{
   class lexerCPP : public QsciLexerCPP
   {
      Q_OBJECT

      public:
                              lexerCPP (QObject* parent = nullptr);
                              ~lexerCPP () override;

         const char*          language() const override;
         QFont                defaultFont (int style) const override;
   };
}
