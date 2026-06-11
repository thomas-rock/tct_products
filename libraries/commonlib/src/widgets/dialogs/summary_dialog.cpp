//=============================================================================
// RockSolid Logic Corporation                         www.RockSolidLogic.com
//
//   This confidential and proprietary code may be used only as
//   authorized by a licensing agreement from RockSolid Solutions.
//   In the event of publication, the following notice is applicable:
//
//        COPYRIGHT © 2011 to the present, RockSolid Solutions
//        ALL RIGHTS RESERVED
//
//   The entire notice above must be reproduced on all authorized copies.
//
//=============================================================================
// Last checked in by $Author: thomas@x-tekcorp.com $   $Revision: 1255 $
//
// $Date: 2016-05-27 08:50:47 -0400 (Fri, 27 May 2016) $
//=============================================================================
// $Id: xTextEditPreferences.h 1255 2016-05-27 12:50:47Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================
#include "summary_dialog.h"
#include "ui_summary_dialog.h"

#include <QFileInfo>
using namespace commonLib;

SummaryDialog::SummaryDialog (QString filename, QWidget* parent)
   : QDialog(parent)
{
   setup();

   QFileInfo fi(filename);
   int permissions = fi.permissions();
   QString perm;
   if ((permissions & QFileDevice::ReadOwner) != 0) perm += "R";
   if ((permissions & QFileDevice::WriteOwner) != 0) perm += "W";
   if ((permissions & QFileDevice::ExeOwner) != 0) perm += "X";

   QString text = QString("Full File Name : %1\n"
                          "Created        : %2\n"
                          "Modified       : %3\n"
                          "File Size      : %4\n"
                          "Permissions    : %5\n").arg(filename)
                                                  .arg(fi.birthTime().toString())
                                                  .arg(fi.lastModified().toString())
                                                  .arg(QString::number(fi.size()))
                                                  .arg(perm);

   m_ui->_textEntry->setPlainText(text);
}
SummaryDialog::~SummaryDialog () {delete m_ui;}
//-----------------------------------------------------------------------------
void SummaryDialog::setup ()
{
   m_ui = new Ui::SummaryDialog;
   m_ui->setupUi(this);
}
