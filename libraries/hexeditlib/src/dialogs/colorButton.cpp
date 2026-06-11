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
// $Id: xColorButton.cpp 1236 2016-05-24 23:21:29Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================

#include "colorButton.h"
#include <QtWidgets>

using namespace hexeditLib;

colorButton::colorButton (QWidget* parent)
   : QToolButton(parent)
{
   setMaximumSize(18, 18);
   _alpha = false;
   _border = false;
   _current = Qt::black;
   connect(this, SIGNAL(clicked()), this, SLOT(getColor()));
   setColor();
}
//-----------------------------------------------------------------------------
QColor colorButton::currentColor () {return _current;}
QString colorButton::currentColorName () {return _current.name(QColor::HexArgb);}
void colorButton::setAlpha (bool state) {_alpha = state;}
void colorButton::setBorder (bool border)
{
   _border = border;
   setColor();
}
//-----------------------------------------------------------------------------
void colorButton::clear ()
{
   setStyleSheet(QString());
   setToolTip(QString());
}
void colorButton::setCurrentColor (QColor c)
{
   _current = c;
   setColor();
}
void colorButton::setEnabled (bool state)
{
   QToolButton::setEnabled(state);

   if (!state)
      setStyleSheet(QString());
   else
      setColor();
}
//-----------------------------------------------------------------------------
void colorButton::getColor ()
{
   QColorDialog dialog;
   dialog.setOption(QColorDialog::ShowAlphaChannel, _alpha);
   dialog.setCurrentColor(_current);
   if (!dialog.exec()) return;

   _current = dialog.selectedColor();
   setColor();
}
//-----------------------------------------------------------------------------
void colorButton::setColor ()
{
   QString style = "background-color: rgb(" + QString::number(_current.red()) + "," +
                                               QString::number(_current.green()) + "," +
                                               QString::number(_current.blue()) + ");"; // +
                                        //       QString::number(_current.alpha()) + ");";

   if (_border)
      style += "border: 2px solid white;";
   setStyleSheet(style);
   setToolTip(_current.name(QColor::HexArgb));
   emit colorChanged(_current);
}
