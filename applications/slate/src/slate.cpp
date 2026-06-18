#include "slate.h"
#include "ui_Slate.h"
#include "settings_p.h"
#include "text_edit.h"
// #include "rtfEdit.h"
// #include "hexEdit.h"
// #include "pdfView.h"
// #include "dotView.h"
// #include "chartView.h"
// #include "imageView.h"
#include "help_page.h"

#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>

#define VSPACER  new QSpacerItem(10, 100, QSizePolicy::Minimum, QSizePolicy::Fixed)
#define VSPACER_STRETCH  new QSpacerItem(10, 100, QSizePolicy::Minimum, QSizePolicy::Expanding)

Slate::Slate(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::Slate)
{
   m_ui->setupUi(this);

   // add spacer to mode toolbar between mode buttons and control buttons
   QWidget* spacer = new QWidget(this);
   QLayout* spacerLayout = new QVBoxLayout;
   spacer->setLayout(spacerLayout);
   spacerLayout->addItem(VSPACER_STRETCH);
   m_ui->modeToolbar->insertWidget(m_ui->exitAct, spacer);
   m_ui->modeToolbar->layout()->setSpacing(10);

   // add mode pages
   // QMainWindow* rtfEdit = new rtfeditLib::rtfEdit;
   // m_ui->modeStack->addWidget(rtfEdit);
   // _modeActs.append(m_ui->rtfModeAct);
   QMainWindow* textEdit = new texteditLib::TextEdit;
   m_ui->modeStack->addWidget(textEdit);
   m_mode_acts.append(m_ui->textModeAct);
   // QMainWindow* hexEdit = new hexeditLib::hexEdit;
   // m_ui->modeStack->addWidget(hexEdit);
   // _modeActs.append(m_ui->hexModeAct);
   // QMainWindow* tableEdit = new QMainWindow;
   // m_ui->modeStack->addWidget(tableEdit);
   // _modeActs.append(m_ui->tableModeAct);
   // QMainWindow* pdfView = new pdfLib::pdfView;
   // m_ui->modeStack->addWidget(pdfView);
   // _modeActs.append(m_ui->pdfModeAct);
   // QMainWindow* dotView = new dotLib::dotView;
   // m_ui->modeStack->addWidget(dotView);
   // _modeActs.append(m_ui->graphModeAct);
   // QMainWindow* chartView = new chartLib::chartView;
   // m_ui->modeStack->addWidget(chartView);
   // _modeActs.append(m_ui->chartModeAct);
   // QMainWindow* imageView = new imageLib::imageView;
   // m_ui->modeStack->addWidget(imageView);
   // _modeActs.append(m_ui->imageModeAct);
   // QMainWindow* svgView = new QMainWindow;
   // m_ui->modeStack->addWidget(svgView);
   // _modeActs.append(m_ui->svgModeAct);
   QMainWindow* helpView = new HelpPage;
   m_ui->modeStack->addWidget(helpView);
   m_mode_acts.append(m_ui->helpModeAct);

   // setup mode controls
   foreach (QAction* mode, m_mode_acts)
      connect(mode,  &QAction::triggered,
              this, &Slate::setMode);

   connect(m_ui->exitAct, &QAction::triggered,
           this,          &Slate::close);

   // style mode buttons
   QFile styleFile(":/styles/modeButton.qss");
   if (styleFile.open(QIODevice::ReadOnly)) {
      QTextStream s(&styleFile);
      QString style = s.readAll();

      foreach (QAction* mode, m_mode_acts)
         m_ui->modeToolbar->widgetForAction(mode)->setStyleSheet(style);

      m_ui->modeToolbar->widgetForAction(m_ui->exitAct)->setStyleSheet(style);
   }

   // restore state
   QSettings settings;
   resize(settings.value(SETTINGS_SIZE, QSize(600, 600)).toSize());
   move(settings.value(SETTINGS_POS, QPoint(200, 200)).toPoint());
   restoreState(settings.value(SETTINGS_STATE).toByteArray());
   QDir::setCurrent(settings.value(SETTINGS_DIR).toString());

   m_mode_acts[0]->trigger();
}

Slate::~Slate() {delete m_ui;}

//-----------------------------------------------------------------------------
void Slate::setMode (bool state)
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
}
//-----------------------------------------------------------------------------
