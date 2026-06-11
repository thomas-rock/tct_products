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
// Last checked in by $Author: thomas@x-tekcorp.com $   $Revision: 937 $
//
// $Date: 2015-02-16 19:27:06 -0500 (Mon, 16 Feb 2015) $
//=============================================================================
// $Id: XRtfEdit.cpp 937 2015-02-17 00:27:06Z thomas@x-tekcorp.com $
//=============================================================================
//
// Description :
//
// Notes :
//
//=============================================================================
#include "rtfEdit.h"
#include "ui_rtfEdit.h"
#include <QSettings>
#include <QCloseEvent>

using namespace rtfeditLib;

rtfEdit::rtfEdit (QWidget* parent) : QMainWindow(parent) {setup();}
rtfEdit::~rtfEdit () {delete _ui;}

//-----------------------------------------------------------------------------
void rtfEdit::edit (const QString& filename, int line, int col) {_ui->editor->edit(filename, line, col);}
//-----------------------------------------------------------------------------
void rtfEdit::updateFileMenu ()
{
   _ui->recentFilesMenu->clear();
   foreach (QString filename, _ui->editor->recentFiles()) {
      _ui->recentFilesMenu->addAction(filename);
   }
   QString filename = _ui->editor->currentFilename();
   bool newfile = filename.startsWith("[");
   _ui->summaryAct->setEnabled(!newfile);
   newfile &= _ui->editor->isPristine();
   _ui->reloadAct->setEnabled(!newfile);
   _ui->saveAct->setEnabled(!newfile);
   _ui->closeAct->setEnabled(!newfile);
}
void rtfEdit::recentSelected (QAction* file) {_ui->editor->fileOpen(file->text());}
void rtfEdit::updateViewMenu () {}
void rtfEdit::updateWindowMenu ()
{
   _ui->windowMenu->clear();
   foreach (QString filename, _ui->editor->filenames()) {
      _ui->windowMenu->addAction(filename);
   }
}
void rtfEdit::editorChanged ()
{
   _filenameLabel->setText(_ui->editor->currentFilename());

   int line, index;
   _ui->editor->cursorPosition(&line, &index);
   cursorPositionChanged(line, index);
   selectionChanged();
   textChanged();
}
void rtfEdit::cursorPositionChanged (int, int) {}
void rtfEdit::selectionChanged ()
{
   selectedCount();

   bool state = _ui->editor->hasSelectedText();
   _ui->copyAct->setEnabled(state);
   _ui->cutAct->setEnabled(state);
   _ui->deleteAct->setEnabled(state);
   _ui->indentAct->setEnabled(state);
   _ui->unindentAct->setEnabled(state);
   _ui->uppercaseAct->setEnabled(state);
   _ui->lowercaseAct->setEnabled(state);
   _ui->capitalizeAct->setEnabled(state);
}
void rtfEdit::selectedCount () {_selectedCountLabel->setText(QString("   Selected Count: %1   ").arg(_ui->editor->selectedCount()));}
void rtfEdit::textChanged ()
{
   _ui->redoAct->setEnabled(_ui->editor->isRedoAvailable());
   _ui->undoAct->setEnabled(_ui->editor->isUndoAvailable());
}
void rtfEdit::contextMenu (QPoint pos) {_contextMenu->exec(pos);}
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
void rtfEdit::setup ()
{
   _ui = new Ui::rtfEdit;
   _ui->setupUi(this);

   _ui->findReplace->hide();

   connect(_ui->editor,                 SIGNAL(editorChanged()),     this,          SLOT(editorChanged()));
   connect(_ui->editor,                 SIGNAL(cursorPositionChanged(int, int)), this, SLOT(cursorPositionChanged(int,int)));
   connect(_ui->editor,                 SIGNAL(selectionChanged()),  this,          SLOT(selectionChanged()));
   connect(_ui->editor,                 SIGNAL(selectedCount(int)),  this,          SLOT(selectedCount()));
   connect(_ui->editor,                 SIGNAL(textChanged()),       this,          SLOT(textChanged()));
   connect(_ui->editor,                 SIGNAL(contextMenuRequest(QPoint)),      this,  SLOT(contextMenu(QPoint)));

   connect(_ui->fileMenu,               SIGNAL(aboutToShow()),       this,          SLOT(updateFileMenu()));
   connect(_ui->recentFilesMenu,        SIGNAL(triggered(QAction*)), this,          SLOT(recentSelected(QAction*)));
   connect(_ui->windowMenu,             SIGNAL(aboutToShow()),       this,          SLOT(updateWindowMenu()));
   connect(_ui->windowMenu,             SIGNAL(triggered(QAction*)), _ui->editor,   SLOT(tabSelected(QAction*)));
   connect(_ui->preferencesAct,         SIGNAL(triggered(bool)),     _ui->editor,   SLOT(preferences()));


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
   connect(_ui->selectWordAct,          SIGNAL(triggered(bool)),     _ui->editor,   SLOT(selectWord()));
   connect(_ui->selectLineAct,          SIGNAL(triggered(bool)),     _ui->editor,   SLOT(selectLine()));
   connect(_ui->selectMatchAct,         SIGNAL(triggered(bool)),     _ui->editor,   SLOT(selectMatch()));
   connect(_ui->selectAllAct,           SIGNAL(triggered(bool)),     _ui->editor,   SLOT(selectAll()));
   connect(_ui->indentAct,              SIGNAL(triggered(bool)),     _ui->editor,   SLOT(indent()));
   connect(_ui->unindentAct,            SIGNAL(triggered(bool)),     _ui->editor,   SLOT(unindent()));
   connect(_ui->uppercaseAct,           SIGNAL(triggered(bool)),     _ui->editor,   SLOT(uppercase()));
   connect(_ui->lowercaseAct,           SIGNAL(triggered(bool)),     _ui->editor,   SLOT(lowercase()));
   connect(_ui->capitalizeAct,          SIGNAL(triggered(bool)),     _ui->editor,   SLOT(capitalize()));
   connect(_ui->changeCaseAct,          SIGNAL(triggered(bool)),     _ui->editor,   SLOT(changeCase()));
   connect(_ui->tabs2SpacesAct,         SIGNAL(triggered(bool)),     _ui->editor,   SLOT(tabs2Spaces()));
   connect(_ui->spaces2TabsAct,         SIGNAL(triggered(bool)),     _ui->editor,   SLOT(spaces2Tabs()));
   connect(_ui->readOnlyAct,            SIGNAL(triggered(bool)),     _ui->editor,   SLOT(setReadOnly()));
   connect(_ui->clearReadOnlyAct,       SIGNAL(triggered(bool)),     _ui->editor,   SLOT(clearReadOnly()));

   connect(_ui->findReplaceAct,         SIGNAL(triggered(bool)),     _ui->findReplace,  SLOT(show()));
   connect(_ui->findReplace,            SIGNAL(find(QString,bool,bool,bool,bool,bool)), _ui->editor, SLOT(find(QString,bool,bool,bool,bool,bool)));
   connect(_ui->findReplace,            SIGNAL(replace(QString,QString,bool,bool,bool,bool,bool,bool,bool)), _ui->editor, SLOT(replace(QString,QString,bool,bool,bool,bool,bool,bool,bool)));
   connect(_ui->editor,                 SIGNAL(replacedCount(int)),  _ui->findReplace,  SLOT(replacedCount(int)));

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
   _contextMenu->addSeparator();
   _contextMenu->addMenu(_ui->selectMenu);
   _contextMenu->addSeparator();
   _contextMenu->addAction(_ui->indentAct);
   _contextMenu->addAction(_ui->unindentAct);
   _contextMenu->addSeparator();
   _contextMenu->addMenu(_ui->changeCaseMenu);

   // setup status bar
   _selectedCountLabel = new QLabel;
   _filenameLabel = new QLabel;
   _ui->statusBar->addPermanentWidget(_filenameLabel, 1);
   _ui->statusBar->addPermanentWidget(_selectedCountLabel);

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

   // create initial empty file
   _ui->editor->fileNew();
}
