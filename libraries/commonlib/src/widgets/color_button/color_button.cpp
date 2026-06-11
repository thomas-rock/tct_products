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

#include "color_button.h"
#include <QtWidgets>

using namespace commonLib;

ColorButton::ColorButton (QWidget* parent)
   : QToolButton(parent)
{
   setMaximumSize(18, 18);
   m_alpha = false;
   m_border = false;
   m_current = Qt::black;
   connect(this, SIGNAL(clicked()), this, SLOT(getColor()));
   setColor();
}
//-----------------------------------------------------------------------------
QColor ColorButton::currentColor () {return m_current;}
QString ColorButton::currentColorName () {return m_current.name(QColor::HexArgb);}
void ColorButton::setAlpha (bool state) {m_alpha = state;}
void ColorButton::setBorder (bool border)
{
   m_border = border;
   setColor();
}
//-----------------------------------------------------------------------------
void ColorButton::clear ()
{
   setStyleSheet(QString());
   setToolTip(QString());
}
void ColorButton::setCurrentColor (QColor c)
{
   m_current = c;
   setColor();
}
void ColorButton::setEnabled (bool state)
{
   QToolButton::setEnabled(state);

   if (!state)
      setStyleSheet(QString());
   else
      setColor();
}
//-----------------------------------------------------------------------------
void ColorButton::getColor ()
{
   QColorDialog dialog;
   dialog.setOption(QColorDialog::ShowAlphaChannel, m_alpha);
   dialog.setCurrentColor(m_current);
   if (!dialog.exec()) return;

   m_current = dialog.selectedColor();
   setColor();
}
//-----------------------------------------------------------------------------
void ColorButton::setColor ()
{
   QString style = "background-color: rgb(" + QString::number(m_current.red()) + "," +
                                               QString::number(m_current.green()) + "," +
                                               QString::number(m_current.blue()) + ");"; // +
                                        //       QString::number(_current.alpha()) + ");";

   if (m_border)
      style += "border: 2px solid white;";
   setStyleSheet(style);
   setToolTip(m_current.name(QColor::HexArgb));
   emit colorChanged(m_current);
}
