#pragma once

#include<Qsci/qscilexerverilog.h>

namespace editorLib
{
   class lexerVerilog : public QsciLexerVerilog
   {
      Q_OBJECT

      public:
                              lexerVerilog (QObject* parent = nullptr);
                              ~lexerVerilog () override;

         const char*          language() const override;
         QFont                defaultFont (int style) const override;
   };
}
