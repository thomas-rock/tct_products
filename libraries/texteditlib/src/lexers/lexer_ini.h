//=============================================================================
// Treasure Coast Technologies, Inc.          www.TreasureCoastTechnologies.com
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from RockSolid Solutions.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2026 to the present, Treasure Coast Technologies, Inc.
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
//
// Description :
//
//=============================================================================
#pragma once

#include<Qsci/qscilexercustom.h>

namespace texteditLib
{
   class LexerINI : public QsciLexerCustom
   {
      Q_OBJECT

      public:
                              LexerINI (QObject* parent = nullptr);
                              ~LexerINI () override;

         const char*          language() const override;
         QString              description (int style) const override;

         void                 styleText (int start, int end) override;
         QColor               defaultColor (int style) const override;
         QFont                defaultFont (int style) const override;

         QStringList extensions () {return {"ini"};}

      protected:
         void                 style (QList<QRegularExpression*> patterns, int start, int style);

      private:
         static QList<QRegularExpression*>   m_groups;
         static QList<QRegularExpression*>   m_keys;
         static QList<QRegularExpression*>   m_comments;

         QString                             m_source;
         QFont                               m_font;

   };
}
