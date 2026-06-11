#include "hexEdit.h"
#include "ui_hexEdit.h"
#include <QSettings>
#include <QCloseEvent>

using namespace hexeditLib;

hexEdit::hexEdit (QWidget* parent) : QMainWindow(parent) {setup();}
hexEdit::~hexEdit() {delete _ui;}
//-----------------------------------------------------------------------------
void hexEdit::edit (const QString& filename, int line, int col) {_ui->editor->fileOpen(filename, line, col);}
//-----------------------------------------------------------------------------
void hexEdit::updateFileMenu ()
{
   _ui->recentFilesMenu->clear();
   foreach (QString filename, _ui->editor->recentFiles()) {
      _ui->recentFilesMenu->addAction(filename);
   }
}
void hexEdit::recentSelected (QAction* file) {_ui->editor->fileOpen(file->text());}
void hexEdit::updateWindowMenu ()
{
   _ui->windowMenu->clear();
   foreach (QString filename, _ui->editor->filenames()) {
      _ui->windowMenu->addAction(filename);
   }
}
void hexEdit::editorChanged ()
{
   _ui->filenameLabel->setText(_ui->editor->currentFilename());
   _ui->addressLabel->setText(QString("0x%1").arg(QString::number(_ui->editor->currentAddress(), 16)));
   _ui->sizeLabel->setText(QString("%1").arg(_ui->editor->currentSize()));
   _ui->modeLabel->setText((_ui->editor->currentMode()) ? "Overwrite" : "Insert");

   dataChanged();
}
void hexEdit::dataChanged ()
{
   _ui->redoAct->setEnabled(_ui->editor->isRedoAvailable());
   _ui->undoAct->setEnabled(_ui->editor->isUndoAvailable());
}
void hexEdit::currentAddressChanged(qint64 addr) {_ui->addressLabel->setText(QString("0x%1").arg(QString::number(addr, 16).toUpper()));}
void hexEdit::currentSizeChanged(qint64 size) {_ui->sizeLabel->setText(QString("%1").arg(size));}
void hexEdit::overwriteModeChanged(bool mode) {_ui->modeLabel->setText((mode) ? "Overwrite" : "Insert");}
void hexEdit::contextMenu (QPoint pos) {_contextMenu->exec(pos);}
//-----------------------------------------------------------------------------
/*
void hexEdit::closeEvent (QCloseEvent* event)
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
void hexEdit::setup ()
{
   _ui = new Ui::hexEdit;
   _ui->setupUi(this);

   _ui->_findReplacePanel->hide();

   connect(_ui->editor,                SIGNAL(editorChanged()),      this,          SLOT(editorChanged()));
   connect(_ui->editor,                SIGNAL(currentAddressChanged(qint64)), this, SLOT(currentAddressChanged(qint64)));
   connect(_ui->editor,                SIGNAL(currentSizeChanged(qint64)),    this, SLOT(currentSizeChanged(qint64)));
   connect(_ui->editor,                SIGNAL(overwriteModeChanged(bool)),    this, SLOT(overwriteModeChanged(bool)));
   connect(_ui->editor,                SIGNAL(dataChanged()),        this,          SLOT(dataChanged()));

   connect(_ui->fileMenu,               SIGNAL(aboutToShow()),       this,          SLOT(updateFileMenu()));
   connect(_ui->recentFilesMenu,        SIGNAL(triggered(QAction*)), this,          SLOT(recentSelected(QAction*)));
   connect(_ui->windowMenu,             SIGNAL(aboutToShow()),       this,          SLOT(updateWindowMenu()));
   connect(_ui->windowMenu,             SIGNAL(triggered(QAction*)), _ui->editor,   SLOT(tabSelected(QAction*)));

   connect(_ui->newAct,                 SIGNAL(triggered(bool)),     _ui->editor,   SLOT(fileNew()));
   connect(_ui->openAct,                SIGNAL(triggered(bool)),     _ui->editor,   SLOT(fileOpen()));
   connect(_ui->reloadAct,              SIGNAL(triggered(bool)),     _ui->editor,   SLOT(fileRevert()));
   connect(_ui->saveAct,                SIGNAL(triggered(bool)),     _ui->editor,   SLOT(fileSave()));
   connect(_ui->saveAsAct,              SIGNAL(triggered(bool)),     _ui->editor,   SLOT(fileSaveAs()));
   connect(_ui->saveAllAct,             SIGNAL(triggered(bool)),     _ui->editor,   SLOT(fileSaveAll()));
   connect(_ui->closeAct,               SIGNAL(triggered(bool)),     _ui->editor,   SLOT(fileClose()));
   connect(_ui->closeAllAct,            SIGNAL(triggered(bool)),     _ui->editor,   SLOT(fileCloseAll()));
   connect(_ui->summaryAct,             SIGNAL(triggered(bool)),     _ui->editor,   SLOT(summary()));
   connect(_ui->printAct,               SIGNAL(triggered(bool)),     _ui->editor,   SLOT(filePrint()));
   connect(_ui->exitAct,                SIGNAL(triggered(bool)),     this,          SLOT(close()));

   connect(_ui->undoAct,                SIGNAL(triggered(bool)),     _ui->editor,   SLOT(undo()));
   connect(_ui->redoAct,                SIGNAL(triggered(bool)),     _ui->editor,   SLOT(redo()));
   connect(_ui->cutAct,                 SIGNAL(triggered(bool)),     _ui->editor,   SLOT(cut()));
   connect(_ui->copyAct,                SIGNAL(triggered(bool)),     _ui->editor,   SLOT(copy()));
   connect(_ui->pasteAct,               SIGNAL(triggered(bool)),     _ui->editor,   SLOT(paste()));
   connect(_ui->deleteAct,              SIGNAL(triggered(bool)),     _ui->editor,   SLOT(del()));
   connect(_ui->preferencesAct,         SIGNAL(triggered(bool)),     _ui->editor,   SLOT(preferences()));

   connect(_ui->findReplaceAct,         SIGNAL(triggered(bool)),     _ui->_findReplacePanel,  SLOT(show()));
   connect(_ui->_findReplacePanel,       SIGNAL(find(QString, bool, bool)), _ui->editor, SLOT(find(QString, bool, bool)));
   connect(_ui->_findReplacePanel,       SIGNAL(replace(QString, bool, QString, bool, bool)), _ui->editor, SLOT(replace(QString, bool, QString, bool, bool)));
//   connect(_ui->gotoAct,                SIGNAL(triggered(bool)),     this,          SLOT(gotoLine()));

   // create editor context menu
   _contextMenu = new QMenu;
   _contextMenu->addAction(_ui->undoAct);
   _contextMenu->addAction(_ui->redoAct);
   _contextMenu->addSeparator();
   _contextMenu->addAction(_ui->cutAct);
   _contextMenu->addAction(_ui->copyAct);
   _contextMenu->addAction(_ui->pasteAct);
   _contextMenu->addSeparator();
   _contextMenu->addAction(_ui->deleteAct);

   editorChanged();        // update for initial editor created in textEditor
}

