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

#include "goto_dialog.h"
#include "ui_goto_dialog.h"

using namespace texteditLib;

GotoDialog::GotoDialog (int maxLine, QWidget* parent, Qt::WindowFlags f)
   : QDialog(parent, f)
{
   m_ui = new Ui::GotoDialog;
   m_ui->setupUi(this);
   m_ui->gotoLabel->setText(QString("Line: 1 - %1").arg(maxLine));
   m_ui->gotoSpin->setRange(1, maxLine);
}
GotoDialog::~GotoDialog () {delete m_ui;}

//-----------------------------------------------------------------------------
int GotoDialog::line () {return m_ui->gotoSpin->value();}
