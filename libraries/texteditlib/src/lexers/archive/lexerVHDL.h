#pragma once

#include<Qsci/qscilexervhdl.h>

namespace editorLib
{
   class lexerVHDL : public QsciLexerVHDL
   {
      Q_OBJECT

      public:
                              lexerVHDL (QObject* parent = nullptr);
                              ~lexerVHDL ();

         const char*          language() const;
         QFont                defaultFont (int style) const;
   };
}
