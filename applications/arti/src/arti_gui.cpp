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
#include "arti_gui.h"
#include "ui_arti_gui.h"
#include "main_page.h"
#include "help_page.h"
#include "settings_p.h"
#include "revision.h"

#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>

#define VSPACER  new QSpacerItem(10, 100, QSizePolicy::Minimum, QSizePolicy::Fixed)
#define VSPACER_STRETCH  new QSpacerItem(10, 100, QSizePolicy::Minimum, QSizePolicy::Expanding)

ArtiGui::ArtiGui(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::ArtiGui)
{
   m_ui->setupUi(this);

   setWindowTitle(QString("%1 - %2").arg(APPNAME, VERSION));

   m_ui->runAct->setEnabled(false);

   // add spacer to mode toolbar between mode buttons and control buttons
   QWidget* spacer = new QWidget(this);
   QLayout* spacerLayout = new QVBoxLayout;
   spacer->setLayout(spacerLayout);
   spacerLayout->addItem(VSPACER_STRETCH);
   m_ui->modeToolbar->insertWidget(m_ui->runAct, spacer);
   m_ui->modeToolbar->layout()->setSpacing(10);

   // add mode pages
   m_main_page = new MainPage;
   m_ui->modeStack->addWidget(m_main_page);
   m_mode_acts.append(m_ui->mainModeAct);

   m_edit_page = new texteditLib::TextEdit;
   m_ui->modeStack->addWidget(m_edit_page);
   m_mode_acts.append(m_ui->textModeAct);

   QMainWindow* help_page = new HelpPage;
   m_ui->modeStack->addWidget(help_page);
   m_mode_acts.append(m_ui->helpModeAct);

   // setup mode controls
   foreach (QAction* mode, m_mode_acts)
      connect(mode,  &QAction::triggered,
              this,  &ArtiGui::setMode);

   // style mode buttons
   QFile styleFile(":/styles/modeButton.qss");
   if (styleFile.open(QIODevice::ReadOnly)) {
      QTextStream s(&styleFile);
      QString style = s.readAll();

      foreach (QAction* mode, m_mode_acts)
         m_ui->modeToolbar->widgetForAction(mode)->setStyleSheet(style);

      m_ui->modeToolbar->widgetForAction(m_ui->checkAct)->setStyleSheet(style);
      m_ui->modeToolbar->widgetForAction(m_ui->runAct)->setStyleSheet(style);
      m_ui->modeToolbar->widgetForAction(m_ui->exitAct)->setStyleSheet(style);
   }

   // setup misc connections
   connect(m_main_page,    &MainPage::initialChanged,
           this,           &ArtiGui::initialChanged);

   connect(m_main_page,    &MainPage::editRequest,
           this,           &ArtiGui::editFile);

   connect(m_ui->runAct,   &QAction::triggered,
           m_main_page,    &MainPage::run);

   connect(m_ui->checkAct, &QAction::triggered,
           m_edit_page,    &texteditLib::TextEdit::check);
\
   connect(m_ui->exitAct,  &QAction::triggered,
           this,           &ArtiGui::close);

   // restore state
   QSettings settings;
   resize(settings.value(SETTINGS_SIZE, QSize(600, 600)).toSize());
   move(settings.value(SETTINGS_POS, QPoint(200, 200)).toPoint());
   restoreState(settings.value(SETTINGS_STATE).toByteArray());
   QDir::setCurrent(settings.value(SETTINGS_DIR).toString());

   // m_main_page->loadState(settings);
   // m_help_page->loadState(settings);

   m_mode_acts[0]->trigger();       // set main mode
}

ArtiGui::~ArtiGui() {delete m_ui;}

//-----------------------------------------------------------------------------
void ArtiGui::setMode (bool state)
{
   QAction* action = qobject_cast<QAction*>(sender());

   if (m_current_mode) {
      if (action == m_current_mode) {
         m_current_mode->setChecked(true);
         return;
      }

      m_current_mode->setChecked(false);
   }

   m_current_mode = action;

   int currentIndex = -1;
   for (int index = 0; index < m_mode_acts.size(); index++) {
      if (action == m_mode_acts[index]) {
         currentIndex = index;
         break;
      }
   }
   m_ui->modeStack->setCurrentIndex(currentIndex);

   m_ui->runAct->setVisible(currentIndex == 0);
   m_ui->checkAct->setVisible(currentIndex == 1);
}

//-----------------------------------------------------------------------------
void ArtiGui::initialChanged (const QString &path)
{
   m_ui->runAct->setEnabled(!path.isEmpty());
}

void ArtiGui:: editFile (QString path)
{
   m_mode_acts[1]->trigger();       // set main mode
   m_edit_page->edit(path);
}

void ArtiGui::closeEvent (QCloseEvent* event)
{
   // save settings
   QSettings settings;
   settings.setValue(SETTINGS_SIZE, size());
   settings.setValue(SETTINGS_POS, pos());
   settings.setValue(SETTINGS_STATE, saveState());
   settings.setValue(SETTINGS_DIR, QDir::currentPath());

   // m_main_page->saveState(settings);
   // m_help_page->saveState(settings);

   event->accept();
}
