//=============================================================================
// X-Tek Corporation                                   http://www.x-tekcorp.com
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from X-Tek Corporation.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2011 to the present, X-Tek Corporation
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
// Last checked in by $Author:: thomas@x-tekcorp.com      $   $Revision: 1236 $
//
// $Date: 2016-05-24 19:21:29 -0400 (Tue, 24 May 2016) $
//=============================================================================
// $Id: xColorButton.h 1236 2016-05-24 23:21:29Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================

#pragma once

#include <QToolButton>

namespace hexeditLib
{
   class colorButton : public QToolButton
   {
      Q_OBJECT

      protected:
         bool           _border;
         QColor         _current;
         bool           _alpha;

      public:
                        colorButton (QWidget* parent = nullptr);

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

   };
}
