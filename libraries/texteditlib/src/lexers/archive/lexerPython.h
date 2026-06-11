#pragma once

#include<Qsci/qscilexerpython.h>

namespace editorLib
{
   class lexerPython : public QsciLexerPython
   {
      Q_OBJECT

      public:
                              lexerPython (QObject* parent = nullptr);
                              ~lexerPython () override;

         const char*          language() const override;
         QFont                defaultFont (int style) const override;
   };
}
