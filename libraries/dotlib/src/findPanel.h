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
#include <QCheckBox>

namespace dotLib
{
   namespace Ui {class findPanel;}

   class findPanel : public QWidget
   {
      Q_OBJECT

      private:
         Ui::findPanel*    _ui;
         QAction*          _viewAction;

      public:
                           findPanel (QWidget* parent = nullptr);
                           ~findPanel () override;

         void              setViewAction (QAction* action);
         void              setFindText (QString text);
         void              setFocus ();

      protected Q_SLOTS:
         void              hide ();
         void              propChanged (QString text);
         void              findNext ();
         void              findAll ();

      Q_SIGNALS:
         void              findAll (QString name, bool ncase, bool nregex,
                                    QString text, bool tcase, bool tregex,
                                    QString prop, QString value, bool vcase, bool vregex);
         void              findNext (QString name, bool ncase, bool nregex,
                                     QString text, bool tcase, bool tregex,
                                     QString prop, QString value, bool vcase, bool vregex);

      private:
         void              setup ();
   };
}
