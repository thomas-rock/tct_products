#pragma once

#include<Qsci/qscilexerxml.h>

namespace editorLib
{
   class lexerXML : public QsciLexerXML
   {
      Q_OBJECT

      public:
                              lexerXML (QObject* parent = nullptr);
                              ~lexerXML () override;

         const char*          language() const override;
         QFont                defaultFont (int style) const override;
   };
}
