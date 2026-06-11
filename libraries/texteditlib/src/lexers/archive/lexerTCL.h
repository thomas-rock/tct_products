#pragma once

#include<Qsci/qscilexertcl.h>

namespace editorLib
{
   class lexerTCL : public QsciLexerTCL
   {
      Q_OBJECT

      public:
                              lexerTCL (QObject* parent = nullptr);
                              ~lexerTCL () override;

         const char*          language() const override;
         QFont                defaultFont (int style) const override;
   };
}
