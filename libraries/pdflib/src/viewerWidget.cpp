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
#include <QFileInfo>
#include <QPdfDocument>
#include <QMessageBox>

using namespace pdfLib;

viewerWidget::viewerWidget (QWidget* parent) : QPdfView(parent) {setPageMode(QPdfView::PageMode::MultiPage);}
viewerWidget::~viewerWidget () {}
//-----------------------------------------------------------------------------
QString viewerWidget::filename () const {return _filename;}
void viewerWidget::view (const QString& filename) {readFile(filename);}
void viewerWidget::setFilename (const QString& filename) {_filename = filename;}
double viewerWidget::zoomFactor () const {return zoomFactor();}
//-----------------------------------------------------------------------------
void viewerWidget::zoomIn () {zoom(1.2);}
void viewerWidget::zoomOut () {zoom(1.0/1.2);}
void viewerWidget::zoom100 () {setZoomFactor(1.0);}
void viewerWidget::zoomFit () { /* default does nothing */ }
//-----------------------------------------------------------------------------
void viewerWidget::readFile (const QString& filename)
{
   QPdfDocument* doc = new QPdfDocument;
   doc->load(filename);
   setDocument(doc);
}

void viewerWidget::zoom (double factor) {setZoomFactor(zoomFactor() * factor);}
