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

#include <QPushButton>

PreferencesDialog::PreferencesDialog (QWidget* parent) : QDialog(parent), m_ui(new Ui::PreferencesDialog)
{
   m_ui->setupUi(this);

   connect(m_ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(defaults()));

   load();
}
PreferencesDialog::~PreferencesDialog() {delete m_ui;}
//-----------------------------------------------------------------------------
int PreferencesDialog::exec ()
{
   int stat = QDialog::exec();
   if (stat == QDialog::Accepted) save();
   return stat;
}
//-----------------------------------------------------------------------------
void PreferencesDialog::defaults ()
{
   m_ui->messagePrefs->setDefaults();
   m_ui->loadLastConfig->setChecked(false);
   m_ui->clearOnCheck->setChecked(true);
   m_ui->clearOnRun->setChecked(true);
   m_ui->editorEdit->clear();
   m_ui->maxLoops->setValue(100);
}
//-----------------------------------------------------------------------------
void PreferencesDialog::load ()
{
   QSettings settings;

   defaults();

   m_ui->loadLastConfig->setChecked(settings.value("loadLastConfig", false).toBool());
   m_ui->clearOnCheck->setChecked(settings.value("clearOnCheck", true).toBool());
   m_ui->clearOnRun->setChecked(settings.value("clearOnRun", true).toBool());
   m_ui->editorEdit->setText(settings.value("externalEditor").toString());
   m_ui->maxLoops->setValue(settings.value("maxLoops", 100).toInt());
   m_ui->messagePrefs->load(settings);
}
void PreferencesDialog::save ()
{
   QSettings settings;

   settings.setValue("loadLastConfig", m_ui->loadLastConfig->isChecked());
   settings.setValue("clearOnCheck", m_ui->clearOnCheck->isChecked());
   settings.setValue("clearOnRun", m_ui->clearOnRun->isChecked());
   settings.setValue("externalEditor", m_ui->editorEdit->text());
   settings.setValue("maxLooops", m_ui->maxLoops->value());
   m_ui->messagePrefs->save(settings);
}
