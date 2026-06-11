//=============================================================================
// Treasure Coast Technologies, Inc.          www.TreasureCoastTechnologies.com
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from RockSolid Solutions.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2026 to the present, Treasure Coast Technologies, Inc.
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
//
// Description :
//
//=============================================================================

#include "color_button.h"
#include <QtWidgets>

using namespace texteditLib;

ColorButton::ColorButton (QWidget* parent)
   : QToolButton(parent)
{
   setMaximumSize(18, 18);
   m_alpha = false;
   m_border = false;
   m_current = Qt::black;

   connect(this, &QToolButton::clicked,
           this, &ColorButton::getColor);

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
                                              QString::number(m_current.blue()) + ");";

   if (m_border)
      style += "border: 2px solid white;";
   setStyleSheet(style);
   setToolTip(m_current.name(QColor::HexArgb));
   emit colorChanged(m_current);
}
