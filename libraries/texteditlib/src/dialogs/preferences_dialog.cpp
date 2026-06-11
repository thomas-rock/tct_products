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

#include "preferences_dialog.h"
#include "ui_preferences_dialog.h"
#include "editor_settings_p.h"

using namespace texteditLib;

PreferencesDialog::PreferencesDialog (QWidget* parent) : QDialog(parent)
{
   m_lexer = nullptr;
   setup();
}

PreferencesDialog::~PreferencesDialog ()
{
   qDeleteAll(m_lexers);
   delete m_ui;
}
//-----------------------------------------------------------------------------
void PreferencesDialog::loadPreferences ()
{
   loadGeneral();
   loadStyles();
}
void PreferencesDialog::savePreferences ()
{
   saveGeneral();
   saveStyles();
}
//-----------------------------------------------------------------------------
void PreferencesDialog::setup ()
{
   m_ui = new Ui::PreferencesDialog;
   m_ui->setupUi(this);

   // set styling
   QFile style(":/styles/tabWidget.txt");
   if (style.open(QIODevice::ReadOnly)) {
      QTextStream s(&style);
      m_ui->tabWidget->setStyleSheet(s.readAll());
   }

   setupGeneral();
   setupStyles();
}
