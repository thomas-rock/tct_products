#include "pdfView.h"
#include "ui_pdfView.h"
//#include "viewerSaveDialog.h"
#include <QFileDialog>

using namespace pdfLib;

pdfView::pdfView (QWidget* parent) : QMainWindow(parent) {setup();}

pdfView::~pdfView () {delete _ui;}
//-----------------------------------------------------------------------------
void pdfView::view (const QString& filename) {_ui->viewer->view(filename);}
//-----------------------------------------------------------------------------
void pdfView::updateFileMenu ()
{
   _ui->recentFilesMenu->clear();
   foreach (QString filename, _ui->viewer->recentFiles()) {
      _ui->recentFilesMenu->addAction(filename);
   }
   QString filename = _ui->viewer->currentFilename();
   bool newfile = filename.startsWith("[");
   _ui->summaryAct->setEnabled(!newfile);
   _ui->closeAct->setEnabled(!newfile);
}
void pdfView::recentSelected (QAction* file) {_ui->viewer->fileOpen(file->text());}
void pdfView::updateViewMenu () {}
void pdfView::updateWindowMenu ()
{
   _ui->windowMenu->clear();
   foreach (QString filename, _ui->viewer->filenames()) {
      _ui->windowMenu->addAction(filename);
   }
}
void pdfView::viewerChanged () {_ui->currentFile->setText(_ui->viewer->currentFilename());}
//-----------------------------------------------------------------------------
/*
void textEdit::closeEvent (QCloseEvent* event)
{
   _ui->editor->fileCloseAll();

   //------------------------------------------------------
   // save size and position
   //------------------------------------------------------
   QSettings settings;
   settings.setValue(SETTINGS_SIZE, size());
   settings.setValue(SETTINGS_POS, pos());
   settings.setValue(SETTINGS_STATE, saveState());
//   settings.setValue(SETTINGS_SPLITTER, _ui->mainSplitter->saveState());
   settings.setValue(SETTINGS_DIR, QDir::currentPath());
   settings.setValue(SETTINGS_RECENT, _ui->editor->recentFiles());

   event->accept();
}
*/
//-----------------------------------------------------------------------------
void pdfView::setup ()
{
   _ui = new Ui::pdfView;
   _ui->setupUi(this);

   connect(_ui->viewer,                 SIGNAL(viewerChanged()),     this,          SLOT(editorChanged()));
   connect(_ui->viewer,                 SIGNAL(contextMenuRequest(QPoint)),         this,  SLOT(contextMenu(QPoint)));

   connect(_ui->fileMenu,               SIGNAL(aboutToShow()),       this,          SLOT(updateFileMenu()));
   connect(_ui->recentFilesMenu,        SIGNAL(triggered(QAction*)), this,          SLOT(recentSelected(QAction*)));
   connect(_ui->windowMenu,             SIGNAL(aboutToShow()),       this,          SLOT(updateWindowMenu()));
   connect(_ui->windowMenu,             SIGNAL(triggered(QAction*)), _ui->viewer,   SLOT(tabSelected(QAction*)));
   connect(_ui->preferencesAct,         SIGNAL(triggered(bool)),     _ui->viewer,   SLOT(preferences()));


   connect(_ui->openAct,                SIGNAL(triggered(bool)),     _ui->viewer,   SLOT(fileOpen()));
   connect(_ui->closeAct,               SIGNAL(triggered(bool)),     _ui->viewer,   SLOT(fileClose()));
   connect(_ui->closeAllAct,            SIGNAL(triggered(bool)),     _ui->viewer,   SLOT(fileCloseAll()));
   connect(_ui->summaryAct,             SIGNAL(triggered(bool)),     _ui->viewer,   SLOT(summary()));
   connect(_ui->printAct,               SIGNAL(triggered(bool)),     _ui->viewer,   SLOT(filePrint()));

   //------------------------------------------------------
   // setup view controls
   //------------------------------------------------------
   connect(_ui->zoomInAct,              SIGNAL(triggered(bool)),   _ui->viewer, SLOT(zoomIn()));
   connect(_ui->zoomOutAct,             SIGNAL(triggered(bool)),   _ui->viewer, SLOT(zoomOut()));
   connect(_ui->zoom100Act,             SIGNAL(triggered(bool)),   _ui->viewer, SLOT(zoom100()));
   connect(_ui->zoomFitAct,             SIGNAL(triggered(bool)),   _ui->viewer, SLOT(zoomFit()));


   //------------------------------------------------------
   // restore size and position
   //------------------------------------------------------
/*
   QSettings settings;
   resize(settings.value(SETTINGS_SIZE, QSize(600, 600)).toSize());
   move(settings.value(SETTINGS_POS, QPoint(200, 200)).toPoint());
   restoreState(settings.value(SETTINGS_STATE).toByteArray());
   QDir::setCurrent(settings.value(SETTINGS_DIR).toString());

   _ui->editor->setRecentFiles(settings.value(SETTINGS_RECENT).toStringList());
*/
}
