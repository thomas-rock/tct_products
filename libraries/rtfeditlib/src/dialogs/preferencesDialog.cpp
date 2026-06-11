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
// Last checked in by $Author: thomas@x-tekcorp.com $   $Revision: 1255 $
//
// $Date: 2016-05-27 08:50:47 -0400 (Fri, 27 May 2016) $
//=============================================================================
// $Id: xTextEditPreferences.cpp 1255 2016-05-27 12:50:47Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================

#include "preferencesDialog.h"
#include "ui_preferencesDialog.h"
#include "editorSettings_p.h"

using namespace texteditLib;

preferencesDialog::preferencesDialog (QWidget* parent) : QDialog(parent)
{
   _lexer = nullptr;
   setup();
}

preferencesDialog::~preferencesDialog ()
{
   qDeleteAll(_lexers);
   delete _ui;
}
//-----------------------------------------------------------------------------
void preferencesDialog::loadPreferences ()
{
   loadGeneral();
   loadStyles();
}
void preferencesDialog::savePreferences ()
{
   saveGeneral();
   saveStyles();
}
//-----------------------------------------------------------------------------
void preferencesDialog::setup ()
{
   _ui = new Ui::preferencesDialog;
   _ui->setupUi(this);

   // set styling
   QFile style(":/styles/tabWidget.txt");
   if (style.open(QIODevice::ReadOnly)) {
      QTextStream s(&style);
      _ui->tabWidget->setStyleSheet(s.readAll());
   }

   setupGeneral();
   setupStyles();
}
