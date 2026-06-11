#pragma once

#include<Qsci/qscilexercustom.h>

namespace editorLib
{
   class lexerINI : public QsciLexerCustom
   {
      Q_OBJECT

      private:
         static QList<QRegularExpression*>   _groups;
         static QList<QRegularExpression*>   _keys;
         static QList<QRegularExpression*>   _comments;

         QString              _source;
         QFont                _font;

      public:
                              lexerINI (QObject* parent = nullptr);
                              ~lexerINI () override;

         const char*          language() const override;
         QString              description (int style) const override;

         void                 styleText (int start, int end) override;
         QColor               defaultColor (int style) const override;
         QFont                defaultFont (int style) const override;

      protected:
         void                 style (QList<QRegularExpression*> patterns, int start, int style);

   };
}
