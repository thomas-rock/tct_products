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

#include <QToolButton>

namespace texteditLib
{
   class ColorButton : public QToolButton
   {
      Q_OBJECT

      public:
                        ColorButton (QWidget* parent = nullptr);

         QColor         currentColor ();
         QString        currentColorName ();
         void           setAlpha (bool state);
         void           setBorder (bool border);

      public Q_SLOTS:
         void           clear ();
         void           setCurrentColor (QColor c);
         void           setEnabled (bool state);

      Q_SIGNALS:
         void           colorChanged (QColor);

      protected Q_SLOTS:
         void           getColor ();
         void           setColor ();

      protected:
         bool           m_border;
         QColor         m_current;
         bool           m_alpha;

   };
}
