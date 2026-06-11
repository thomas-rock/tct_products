#pragma once

#include<Qsci/qscilexercustom.h>

namespace editorLib
{
   class lexerJson : public QsciLexerCustom
   {
      Q_OBJECT

      private:
         static QList<QRegularExpression*>   _keywords;
         static QList<QRegularExpression*>   _numbers;
         static QList<QRegularExpression*>   _strings;

         QString              _source;
         QFont                _font;

      public:
                              lexerJson (QObject* parent = nullptr);
                              ~lexerJson () override;

         const char*          language() const override;
         QString              description (int style) const override;

         void                 styleText (int start, int end) override;
         QColor               defaultColor (int style) const override;
         QFont                defaultFont (int style) const override;

      protected:
         void                 style (QList<QRegularExpression*> patterns, int start, int style);

   };
}
