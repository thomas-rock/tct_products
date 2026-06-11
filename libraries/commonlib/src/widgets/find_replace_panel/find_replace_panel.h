//=============================================================================
// RockSolid Logic Corporation                         www.RockSolidLogic.com  
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from RockSolid Solutions.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2011 to the present, RockSolid Solutions
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
// Last checked in by $Author:: thomas@x-tekcorp.com      $   $Revision: 1236 $
//
// $Date: 2016-05-24 19:21:29 -0400 (Tue, 24 May 2016) $
//=============================================================================
// $Id: xFindReplacePanel.h 1236 2016-05-24 23:21:29Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================
#pragma once

#include <QWidget>
#include <QMenu>
#include <QCheckBox>
#include "commonlib_global.h"

namespace commonLib
{
   namespace Ui {class FindReplacePanel;}

   class COMMONLIB_EXPORT FindReplacePanel : public QWidget
   {
      Q_OBJECT

      public:
                              FindReplacePanel (QWidget* parent = nullptr);
                              ~FindReplacePanel () override;

         void                 setFindText (QString text);
         void                 setFocus ();

      public Q_SLOTS:
         void                 replacedCount (int count);

      Q_SIGNALS:
         void                 find (QString text, bool dir, bool cs, bool re, bool wo, bool wrap);
         void                 replace (QString text, QString newtext, bool all, bool next, bool dir, bool cs, bool re, bool wo, bool wrap);

      protected Q_SLOTS:
         void                 findNext ();
         void                 findPrevious ();
         void                 replaceNext ();
         void                 replaceFindNext ();
         void                 replaceAll ();

      private:
         void                 setup ();

      private:
      Ui::FindReplacePanel*   m_ui;
   };
}
