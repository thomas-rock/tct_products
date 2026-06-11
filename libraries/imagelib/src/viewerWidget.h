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

#include <QLabel>
#include <QImage>
#include <QScrollArea>
#include <QWidget>
#include "imagelib_global.h"

namespace imageLib
{
   class IMAGELIB_EXPORT viewerWidget : public QWidget
   {
      Q_OBJECT

      protected:
         QString           _filename;
         QImage            _image;
         QScrollArea*      _scrollArea;
         QLabel*           _imageLabel;
         double            _zoomFactor = 1.0;

      public:
                           viewerWidget (QWidget* parent = nullptr);
         virtual           ~viewerWidget () override;

         QString           filename () const;
         void              view (const QString& filename);
         void              setFilename (const QString& filename);
         double            zoomFactor () const;

      public slots:
         void              zoomIn ();
         void              zoomOut ();
         void              zoom100 ();
         void              zoomFit ();

      protected:
         void              readFile (const QString& filename);
         void              zoom (double factor);
         void              adjustScrollBar(QScrollBar* scrollBar, double factor);
   };
}
