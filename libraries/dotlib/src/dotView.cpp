#include "dotView.h"
#include "ui_dotView.h"
#include "settings_p.h"
#include <QSettings>
#include <QWidgetAction>
#include <QFileDialog>
#include <QTabBar>
#include <QCloseEvent>

using namespace dotLib;

dotView::dotView (QWidget* parent) : QMainWindow(parent) {setup();}
dotView::~dotView () {delete _ui;}
//-----------------------------------------------------------------------------
void dotView::viewFile (const QString& filename) {_ui->viewer->fileOpen(filename);}
void dotView::viewDot (const QString& dot, const QString& title) {}
//-----------------------------------------------------------------------------
void dotView::updateFileMenu ()
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
void dotView::recentSelected (QAction* file) {_ui->viewer->fileOpen(file->text());}
void dotView::updateViewMenu () {}
void dotView::updateWindowMenu ()
{
   _ui->windowMenu->clear();
   foreach (QString filename, _ui->viewer->filenames()) {
      _ui->windowMenu->addAction(filename);
   }
}
void dotView::viewerChanged () {_ui->currentFile->setText(_ui->viewer->currentFilename());}
//-----------------------------------------------------------------------------
void dotView::setup ()
{
   _ui = new dotLib::Ui::dotView;
   _ui->setupUi(this);

   //------------------------------------------------------
   // setup file controls
   //------------------------------------------------------
   // file connections
   connect(_ui->openAct,           SIGNAL(triggered()),       _ui->viewer, SLOT(fileOpen()));
   connect(_ui->closeAct,          SIGNAL(triggered()),       _ui->viewer, SLOT(fileClose()));
   connect(_ui->closeAllAct,       SIGNAL(triggered()),       _ui->viewer, SLOT(fileCloseAll()));
   connect(_ui->printAct,          SIGNAL(triggered()),       _ui->viewer, SLOT(filePrint()));

   //------------------------------------------------------
   // setup view controls
   //------------------------------------------------------
   connect(_ui->zoomInAct,         SIGNAL(triggered(bool)),   _ui->viewer, SLOT(zoomIn()));
   connect(_ui->zoomOutAct,        SIGNAL(triggered(bool)),   _ui->viewer, SLOT(zoomOut()));
   connect(_ui->zoom100Act,        SIGNAL(triggered(bool)),   _ui->viewer, SLOT(zoom100()));
   connect(_ui->zoomFitAct,        SIGNAL(triggered(bool)),   _ui->viewer, SLOT(zoomFit()));

   //------------------------------------------------------
   // setup find panel and controls
   //------------------------------------------------------
   _ui->_findPanel->setViewAction(_ui->findAct);
   connect(_ui->find2Act,    SIGNAL(triggered()),                      this, SLOT(find()));
   connect(_ui->_findPanel,   SIGNAL(findNext(QString,bool,bool,QString,bool,bool,QString,QString,bool,bool)),
           this,              SLOT(findNext(QString,bool,bool,QString,bool,bool,QString,QString,bool,bool)));
   connect(_ui->_findPanel,   SIGNAL(findAll(QString,bool,bool,QString,bool,bool,QString,QString,bool,bool)),
           this,              SLOT(findAll(QString,bool,bool,QString,bool,bool,QString,QString,bool,bool)));

   //------------------------------------------------------
   // setup window controls
   //------------------------------------------------------
   connect(_ui->windowMenu,        SIGNAL(aboutToShow()),     this, SLOT(updateWindowMenu()));
   connect(_ui->windowMenu,        SIGNAL(triggered(QAction*)),this, SLOT(fileOpen(QAction*)));
}
