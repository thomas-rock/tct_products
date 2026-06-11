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
#include <QTextStream>
#include <QLayout>
#include <QMessageBox>
#include <QImageReader>
#include <QScrollBar>

using namespace imageLib;

viewerWidget::viewerWidget (QWidget* parent) : QWidget(parent)
{
   QVBoxLayout* layout = new QVBoxLayout;
   layout->setContentsMargins(0, 0, 0, 0);
   setLayout(layout);
}
viewerWidget::~viewerWidget () {}
//-----------------------------------------------------------------------------
QString viewerWidget::filename () const {return _filename;}
void viewerWidget::view (const QString& filename) {readFile(filename);}
void viewerWidget::setFilename (const QString& filename) {_filename = filename;}
double viewerWidget::zoomFactor () const {return _zoomFactor;}
//-----------------------------------------------------------------------------
void viewerWidget::zoomIn () {zoom(1.2);}
void viewerWidget::zoomOut () {zoom(1.0/1.2);}
void viewerWidget::zoom100 () {_zoomFactor = 1.0; zoom(1.0);}
void viewerWidget::zoomFit () { /* default does nothing */ }
//-----------------------------------------------------------------------------
void viewerWidget::readFile (const QString& filename)
{
   _imageLabel = new QLabel;
   _imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   _imageLabel->setScaledContents(true);

   _image = QImage(filename);
   _imageLabel->setPixmap(QPixmap::fromImage(_image));

   _scrollArea = new QScrollArea;
   _scrollArea->setBackgroundRole(QPalette::Dark);
   _scrollArea->setWidget(_imageLabel);

   layout()->addWidget(_scrollArea);
}
void viewerWidget::zoom (double factor)
{
   _zoomFactor *= factor;
   _imageLabel->resize(_zoomFactor * _imageLabel->pixmap(Qt::ReturnByValue).size());

   adjustScrollBar(_scrollArea->horizontalScrollBar(), factor);
   adjustScrollBar(_scrollArea->verticalScrollBar(), factor);
}
void viewerWidget::adjustScrollBar(QScrollBar* scrollBar, double factor) {scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));}
