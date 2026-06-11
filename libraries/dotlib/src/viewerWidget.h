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
// Last checked in by $Author: thomas@x-tekcorp.com $   $Revision: 1365 $
//
// $Date: 2017-01-18 11:59:18 -0500 (Wed, 18 Jan 2017) $
//=============================================================================
// $Id: xDotWindow.h 1365 2017-01-18 16:59:18Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//============================================================================
#pragma once

#include <QGraphicsView>
#include "dotlib_global.h"

#define DOT_OK       0
#define FILE_ERROR   1
#define DOT_ERROR    2

namespace dotLib
{
   class sceneWidget;
   class DOTLIB_EXPORT viewerWidget : public QGraphicsView
   {
      Q_OBJECT

      protected:
         sceneWidget*      _scene;
         bool              _shiftPressed = false;
         QString           _filename;

      public:
                           viewerWidget (QWidget *parent = nullptr);
         virtual           ~viewerWidget () override;

         QString           filename () const;
         void              setFilename (const QString& filename);
         void              setDot (const QString& dot);

         // search functions
         void              find ();
         void              findAll (QString name, bool ncase, bool nregex,
                                    QString text, bool tcase, bool tregex,
                                    QString prop, QString value, bool vcase, bool vregex);
         void              findNext (QString name, bool ncase, bool nregex,
                                     QString text, bool tcase, bool tregex,
                                     QString prop, QString value, bool vcase, bool vregex);

         void              zoomIn ();
         void              zoomOut ();
         void              zoomFit ();
         void              zoom100 ();
//         void              zoomArea (QRectF area, bool limit100 = true);

      protected slots:
         void              keyPressEvent (QKeyEvent* event) override;
         void              keyReleaseEvent (QKeyEvent* event) override;
         void              wheelEvent (QWheelEvent* event) override;
   };
}
