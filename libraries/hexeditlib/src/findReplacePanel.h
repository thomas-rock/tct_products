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

namespace Ui {class findReplacePanel;}

class findReplacePanel : public QWidget
{
   Q_OBJECT

   private:
      Ui::findReplacePanel*   _ui;

   public:
                  findReplacePanel (QWidget* parent = nullptr);
                  ~findReplacePanel () override;

      void        setFindText (QString text);
      void        setFocus ();

   protected slots:
      void        findNext ();
      void        findPrevious ();
      void        replaceNext ();
      void        replaceFindNext ();
      void        replaceAll ();

   signals:
      void        find (QString text, bool hex, bool rev);
      void        replace (QString ftext, bool fhex, QString rtext, bool rhex, bool rev);
      void        replaceAll (QString ftext, bool fhex, QString rtext, bool rhex);

   private:
      void        setup ();
};
