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
// $Id: xDotWindow.cpp 1365 2017-01-18 16:59:18Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================

#include "viewerWidget.h"
#include "sceneWidget.h"
#include <gvc.h>

using namespace dotLib;

viewerWidget::viewerWidget (QWidget* parent)
   : QGraphicsView(parent)
{
   setRenderHint(QPainter::Antialiasing);
   setRenderHint(QPainter::TextAntialiasing);
   setRenderHint(QPainter::SmoothPixmapTransform);
   setResizeAnchor(QGraphicsView::AnchorViewCenter);

   _scene = new sceneWidget;
   setScene(_scene);
}
viewerWidget::~viewerWidget () {delete _scene;}
//-----------------------------------------------------------------------------
QString viewerWidget::filename () const {return _filename;}
void viewerWidget::setFilename (const QString& filename) {_filename = filename;}
void viewerWidget::setDot (const QString& dot) {_scene->render(dot);}
//-----------------------------------------------------------------------------
void viewerWidget::find () { /* default does nothing */ }
void viewerWidget::findAll (QString name, bool ncase, bool nregex,
                            QString text, bool tcase, bool tregex,
                            QString prop, QString value, bool vcase, bool vregex)
{
   Q_UNUSED(name)
   Q_UNUSED(ncase)
   Q_UNUSED(nregex)
   Q_UNUSED(text)
   Q_UNUSED(tcase)
   Q_UNUSED(tregex)
   Q_UNUSED(prop)
   Q_UNUSED(value)
   Q_UNUSED(vcase)
   Q_UNUSED(vregex)

   // default does nothing
}

void viewerWidget::findNext (QString name, bool ncase, bool nregex,
                             QString text, bool tcase, bool tregex,
                             QString prop, QString value, bool vcase, bool vregex)
{
   Q_UNUSED(name)
   Q_UNUSED(ncase)
   Q_UNUSED(nregex)
   Q_UNUSED(text)
   Q_UNUSED(tcase)
   Q_UNUSED(tregex)
   Q_UNUSED(prop)
   Q_UNUSED(value)
   Q_UNUSED(vcase)
   Q_UNUSED(vregex)

   // default does nothing
}
//-----------------------------------------------------------------------------
void viewerWidget::zoomIn () {qreal factor = 1.2; scale(factor, factor);}
void viewerWidget::zoomOut () {qreal factor = 1.0/1.2; scale(factor, factor);}
void viewerWidget::zoomFit () {fitInView(sceneRect(), Qt::KeepAspectRatio);}
void viewerWidget::zoom100 ()
{
   QTransform trans = transform();
   trans.setMatrix(1.0, trans.m12(), trans.m13(), trans.m21(), 1.0, trans.m23(), trans.m31(), trans.m32(), trans.m33());
   setTransform(trans);
}
//void dotView::zoomArea (QRectF area, bool limit100)
//{
//   _view->fitInView(area, Qt::KeepAspectRatio);
//   QTransform trans = _view->transform();
//
//   if (limit100 && (trans.m11() > 1.0 || trans.m22() > 1.0))
//      zoom100();
//}
//-----------------------------------------------------------------------------
void viewerWidget::keyPressEvent (QKeyEvent* event)
{
   switch (event->key())
   {
      case Qt::Key_Plus    : zoomIn();             break;
      case Qt::Key_Minus   : zoomOut();            break;
      case Qt::Key_0       : zoomFit();            break;
      case Qt::Key_1       : zoom100();            break;
      case Qt::Key_Shift   : _shiftPressed = true; break;         // okay to do default after this - no break needed
   }
}
void viewerWidget::keyReleaseEvent (QKeyEvent* event)
{
   switch (event->key()) {
      case Qt::Key_Shift   : _shiftPressed = false; break;         // okay to do default after this - no break needed
   }
}
void viewerWidget::wheelEvent (QWheelEvent* event)
{
   if (_shiftPressed) {
      setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
      qreal factor = qPow(1.2, event->angleDelta().y() / 240.0);
      scale(factor, factor);
      setResizeAnchor(QGraphicsView::AnchorViewCenter);
      event->accept();
   }
}

