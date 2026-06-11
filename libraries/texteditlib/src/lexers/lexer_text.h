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
   class LexerText : public QsciLexerCustom
   {
      Q_OBJECT

      public:
                              LexerText (QObject* parent = nullptr);
                              ~LexerText () override;

         const char*          language() const override;
         QString              description (int style) const override;

         void                 styleText (int start, int end) override;
         QColor               defaultColor (int style) const override;
         QFont                defaultFont (int style) const override;

         QStringList extensions () {return {"txt", "text"};}

      protected:
         void                 style (QList<QRegularExpression*> patterns, int start, int style);

      private:
         QString              m_source;
         QFont                m_font;

   };
}
