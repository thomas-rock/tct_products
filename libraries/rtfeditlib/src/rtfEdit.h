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
// Last checked in by $Author: thomas@x-tekcorp.com $   $Revision: 937 $
//
// $Date: 2015-02-16 19:27:06 -0500 (Mon, 16 Feb 2015) $
//=============================================================================
// $Id: XRtfEdit.h 937 2015-02-17 00:27:06Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================
#pragma once

#include "rtfeditlib_global.h"
#include <QMainWindow>
#include <QLabel>

namespace rtfeditLib
{
   namespace Ui {class rtfEdit;}

   class RTFEDITLIB_EXPORT rtfEdit : public QMainWindow
   {
      Q_OBJECT

      private:
         Ui::rtfEdit*      _ui;

         QLabel*           _positionLabel;
         QLabel*           _filenameLabel;
         QLabel*           _selectedCountLabel;
         QMenu*            _contextMenu;

      public:
                           rtfEdit (QWidget* parent = nullptr);
                           ~rtfEdit () override;

      public Q_SLOTS:
         void              edit (const QString& filename, int line = 1, int col = 1);        // external access to open a file

      protected Q_SLOTS:
         void              updateFileMenu ();
         void              recentSelected (QAction* file);
         void              updateViewMenu ();
         void              updateWindowMenu ();
         void              editorChanged ();
         void              cursorPositionChanged (int, int);
         void              selectionChanged ();
         void              selectedCount ();
         void              textChanged ();
         void              contextMenu (QPoint pos);

      private:
         void              setup ();
      };
}
