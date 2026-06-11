#pragma once

#include<Qsci/qscilexerhtml.h>

namespace editorLib
{
   class lexerHTML : public QsciLexerHTML
   {
      Q_OBJECT

      public:
                              lexerHTML (QObject* parent = nullptr);
                              ~lexerHTML () override;

         const char*          language() const override;
         QFont                defaultFont (int style) const override;
   };
}
