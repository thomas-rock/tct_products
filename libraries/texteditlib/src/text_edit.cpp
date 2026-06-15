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
#include "text_edit.h"
#include "ui_text_edit.h"
#include <QSettings>
#include <QCloseEvent>

using namespace texteditLib;

TextEdit::TextEdit (QWidget* parent) : QMainWindow(parent) {setup();}
TextEdit::~TextEdit () {delete m_ui;}
//-----------------------------------------------------------------------------
void TextEdit::edit (const QString& filename, int line, int col) {m_ui->editor->edit(filename, line, col);}
void TextEdit::check ()
{
   m_ui->transcript->clear();
   m_ui->editor->check();
}
//-----------------------------------------------------------------------------
void TextEdit::updateFileMenu ()
{
   m_ui->recentFilesMenu->clear();
   foreach (QString filename, m_ui->editor->recentFiles()) {
      m_ui->recentFilesMenu->addAction(filename);
   }
   QString filename = m_ui->editor->currentFilename();
   bool newfile = filename.startsWith("[");
   m_ui->summaryAct->setEnabled(!newfile);
   newfile &= m_ui->editor->isPristine();
   m_ui->reloadAct->setEnabled(!newfile);
   m_ui->saveAct->setEnabled(!newfile);
   m_ui->closeAct->setEnabled(!newfile);
}
void TextEdit::recentSelected (QAction* file) {m_ui->editor->fileOpen(file->text());}
void TextEdit::updateViewMenu ()
{
   QSignalBlocker lineNumbers(m_ui->lineNumbersAct);
   QSignalBlocker markers(m_ui->markersAct);
   QSignalBlocker changeBars(m_ui->changeBarsAct);
   QSignalBlocker indentationGuides(m_ui->indentationGuidesAct);
   QSignalBlocker whitespace(m_ui->whitespaceAct);
   QSignalBlocker endOfLines(m_ui->endOfLinesAct);
   QSignalBlocker currentLineHighlight(m_ui->currentLineHighlightAct);
   QSignalBlocker matchingBraces(m_ui->matchingBracesAct);

   m_ui->lineNumbersAct->setChecked(m_ui->editor->lineNumbers());
   m_ui->markersAct->setChecked(m_ui->editor->markers());
   m_ui->changeBarsAct->setChecked(m_ui->editor->changeBars());
   m_ui->indentationGuidesAct->setChecked(m_ui->editor->indentation());
   m_ui->whitespaceAct->setChecked(m_ui->editor->whitespace());
   m_ui->endOfLinesAct->setChecked(m_ui->editor->eols());
   m_ui->currentLineHighlightAct->setChecked(m_ui->editor->highlight());
   m_ui->matchingBracesAct->setChecked(m_ui->editor->match());
}
void TextEdit::updateWindowMenu ()
{
   m_ui->windowMenu->clear();
   foreach (QString filename, m_ui->editor->filenames()) {
      m_ui->windowMenu->addAction(filename);
   }
}
void TextEdit::editorChanged ()
{
   m_filename_label->setText(m_ui->editor->currentFilename());
   QString lang = m_ui->editor->currentLanguage();
   m_language_label->setText(QString("   %1   ").arg(lang));
   foreach (QAction* act, m_ui->languageMenu->actions())
      act->setChecked(act->text() == lang);

   int line, index;
   m_ui->editor->cursorPosition(&line, &index);
   cursorPositionChanged(line, index);
   selectionChanged();
   textChanged();
}
void TextEdit::cursorPositionChanged (int line, int index) {m_position_label->setText(QString("   Line: %1   Col: %2   ").arg(line+1).arg(index+1));}
void TextEdit::selectionChanged ()
{
   selectedCount();

   bool state = m_ui->editor->hasSelectedText();
   m_ui->copyAct->setEnabled(state);
   m_ui->cutAct->setEnabled(state);
   m_ui->deleteAct->setEnabled(state);
   m_ui->indentAct->setEnabled(state);
   m_ui->unindentAct->setEnabled(state);
   m_ui->commentAct->setEnabled(state);
   m_ui->uncommentAct->setEnabled(state);
   m_ui->uppercaseAct->setEnabled(state);
   m_ui->lowercaseAct->setEnabled(state);
   m_ui->capitalizeAct->setEnabled(state);
}
void TextEdit::selectedCount () {m_selected_count_label->setText(QString("   Selected Count: %1   ").arg(m_ui->editor->selectedCount()));}
void TextEdit::textChanged ()
{
   m_ui->redoAct->setEnabled(m_ui->editor->isRedoAvailable());
   m_ui->undoAct->setEnabled(m_ui->editor->isUndoAvailable());
}
void TextEdit::contextMenu (QPoint pos) {m_context_menu->exec(pos);}
//-----------------------------------------------------------------------------
/*
void TextEdit::closeEvent (QCloseEvent* event)
{
   m_ui->editor->fileCloseAll();

   //------------------------------------------------------
   // save size and position
   //------------------------------------------------------
   QSettings settings;
   settings.setValue(SETTINGS_SIZE, size());
   settings.setValue(SETTINGS_POS, pos());
   settings.setValue(SETTINGS_STATE, saveState());
//   settings.setValue(SETTINGS_SPLITTER, m_ui->mainSplitter->saveState());
   settings.setValue(SETTINGS_DIR, QDir::currentPath());
   settings.setValue(SETTINGS_RECENT, m_ui->editor->recentFiles());

   event->accept();
}
*/

void TextEdit::setTranscriptVisible (bool state)
{
   m_ui->transcript->setVisible(state);
}

//-----------------------------------------------------------------------------
void TextEdit::setup ()
{
   m_ui = new Ui::TextEdit;
   m_ui->setupUi(this);

   m_ui->findReplace->hide();
   m_ui->showTranscriptAct->setChecked(true);

   connect(m_ui->editor,                 &EditorTab::editorChanged,           this,                &TextEdit::editorChanged);
   connect(m_ui->editor,                 &EditorTab::cursorPositionChanged,   this,                &TextEdit::cursorPositionChanged);
   connect(m_ui->editor,                 &EditorTab::selectionChanged,        this,                &TextEdit::selectionChanged);
   connect(m_ui->editor,                 SIGNAL(selectedCount(int)),          this,                SLOT(selectedCount()));
   connect(m_ui->editor,                 &EditorTab::textChanged,             this,                &TextEdit::textChanged);
   connect(m_ui->editor,                 &EditorTab::contextMenuRequest,      this,                &TextEdit::contextMenu);
   connect(m_ui->editor,                 SIGNAL(message(MessageType,QString,QString,int,int)),
           m_ui->transcript,             SLOT(addMessage(MessageType,QString,QString,int,int)));
   connect(m_ui->editor,                 SIGNAL(message(MessageType,QString)),
           m_ui->transcript,             SLOT(addMessage(MessageType,QString)));
   connect(m_ui->transcript,             &commonLib::ListTranscript::positionRequest,
           m_ui->editor,                 &EditorTab::edit);
   connect(m_ui->showTranscriptAct,      &QAction::triggered,                 m_ui->transcriptGroup, &QGroupBox::setVisible);

   connect(m_ui->fileMenu,               SIGNAL(aboutToShow()),       this,          SLOT(updateFileMenu()));
   connect(m_ui->recentFilesMenu,        SIGNAL(triggered(QAction*)), this,          SLOT(recentSelected(QAction*)));
   connect(m_ui->viewMenu,               SIGNAL(aboutToShow()),       this,          SLOT(updateViewMenu()));
   connect(m_ui->languageMenu,           SIGNAL(triggered(QAction*)), m_ui->editor,   SLOT(languageSelected(QAction*)));
   connect(m_ui->windowMenu,             SIGNAL(aboutToShow()),       this,          SLOT(updateWindowMenu()));
   connect(m_ui->windowMenu,             SIGNAL(triggered(QAction*)), m_ui->editor,   SLOT(tabSelected(QAction*)));
   connect(m_ui->preferencesAct,         SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(preferences()));


   connect(m_ui->newAct,                 SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(fileNew()));
   connect(m_ui->openAct,                SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(fileOpen()));
   connect(m_ui->reloadAct,              SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(fileRevert()));
   connect(m_ui->saveAct,                SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(fileSave()));
   connect(m_ui->saveAsAct,              SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(fileSaveAs()));
   connect(m_ui->saveAllAct,             SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(fileSaveAll()));
   connect(m_ui->closeAct,               SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(fileClose()));
   connect(m_ui->closeAllAct,            SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(fileCloseAll()));
   connect(m_ui->summaryAct,             SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(summary()));
   connect(m_ui->printAct,               SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(filePrint()));
   connect(m_ui->exitAct,                SIGNAL(triggered(bool)),     this,          SLOT(close()));

   connect(m_ui->undoAct,                SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(undo()));
   connect(m_ui->redoAct,                SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(redo()));
   connect(m_ui->cutAct,                 SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(cut()));
   connect(m_ui->copyAct,                SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(copy()));
   connect(m_ui->pasteAct,               SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(paste()));
   connect(m_ui->deleteAct,              SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(del()));
   connect(m_ui->selectWordAct,          SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(selectWord()));
   connect(m_ui->selectLineAct,          SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(selectLine()));
   connect(m_ui->selectMatchAct,         SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(selectMatch()));
   connect(m_ui->selectAllAct,           SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(selectAll()));
   connect(m_ui->indentAct,              SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(indent()));
   connect(m_ui->unindentAct,            SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(unindent()));
   connect(m_ui->uppercaseAct,           SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(uppercase()));
   connect(m_ui->lowercaseAct,           SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(lowercase()));
   connect(m_ui->capitalizeAct,          SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(capitalize()));
   connect(m_ui->changeCaseAct,          SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(changeCase()));
   connect(m_ui->commentAct,             SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(comment()));
   connect(m_ui->uncommentAct,           SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(uncomment()));
   connect(m_ui->windowsEolAct,          SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(windowsEol()));
   connect(m_ui->unixEolAct,             SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(unixEol()));
   connect(m_ui->macEolAct,              SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(macEol()));
   connect(m_ui->trimLeadingAct,         SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(trimLeading()));
   connect(m_ui->trimTrailingAct,        SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(trimTrailing()));
   connect(m_ui->trimLeadingTrailingAct, SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(trimLeadingTrailing()));
   connect(m_ui->tabs2SpacesAct,         SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(tabs2Spaces()));
   connect(m_ui->spaces2TabsAct,         SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(spaces2Tabs()));
   connect(m_ui->readOnlyAct,            SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(setReadOnly()));
   connect(m_ui->clearReadOnlyAct,       SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(clearReadOnly()));

   connect(m_ui->findReplaceAct,         SIGNAL(triggered(bool)),     m_ui->findReplace,  SLOT(show()));
   connect(m_ui->findReplace,       SIGNAL(find(QString,bool,bool,bool,bool,bool)), m_ui->editor, SLOT(find(QString,bool,bool,bool,bool,bool)));
   connect(m_ui->findReplace,       SIGNAL(replace(QString,QString,bool,bool,bool,bool,bool,bool,bool)), m_ui->editor, SLOT(replace(QString,QString,bool,bool,bool,bool,bool,bool,bool)));
   connect(m_ui->editor,                 SIGNAL(replacedCount(int)),  m_ui->findReplace,  SLOT(replacedCount(int)));
   connect(m_ui->gotoAct,                SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(gotoLine()));
   connect(m_ui->gotoMatchAct,           SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(gotoMatch()));
   connect(m_ui->nextMarkerAct,          SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(nextMarker()));
   connect(m_ui->prevMarkerAct,          SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(prevMarker()));
   connect(m_ui->clearMarkersAct,        SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(clearMarkers()));

   connect(m_ui->lineNumbersAct,         SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(lineNumbers(bool)));
   connect(m_ui->markersAct,             SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(markers(bool)));
   connect(m_ui->changeBarsAct,          SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(changeBars(bool)));
   connect(m_ui->indentationGuidesAct,   SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(indentation(bool)));
   connect(m_ui->whitespaceAct,          SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(whitespace(bool)));
   connect(m_ui->endOfLinesAct,          SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(eols(bool)));
   connect(m_ui->currentLineHighlightAct,SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(highlight(bool)));
   connect(m_ui->matchingBracesAct,      SIGNAL(triggered(bool)),     m_ui->editor,   SLOT(match(bool)));

   // create editor context menu
   m_context_menu = new QMenu;
   m_context_menu->addAction(m_ui->undoAct);
   m_context_menu->addAction(m_ui->redoAct);
   m_context_menu->addSeparator();
   m_context_menu->addAction(m_ui->cutAct);
   m_context_menu->addAction(m_ui->copyAct);
   m_context_menu->addAction(m_ui->pasteAct);
   m_context_menu->addSeparator();
   m_context_menu->addAction(m_ui->deleteAct);
   m_context_menu->addSeparator();
   m_context_menu->addMenu(m_ui->selectMenu);
   m_context_menu->addSeparator();
   m_context_menu->addAction(m_ui->indentAct);
   m_context_menu->addAction(m_ui->unindentAct);
   m_context_menu->addSeparator();
   m_context_menu->addMenu(m_ui->changeCaseMenu);
   m_context_menu->addSeparator();
   m_context_menu->addAction(m_ui->commentAct);
   m_context_menu->addAction(m_ui->uncommentAct);
//   m_context_menu->addSeparator();
//   m_context_menu->addAction(m_ui->_checkAct);

   // setup status bar
   m_position_label = new QLabel;
   m_language_label = new QLabel;
   m_selected_count_label = new QLabel;
   m_filename_label = new QLabel;
   m_ui->statusBar->addPermanentWidget(m_filename_label, 1);
   m_ui->statusBar->addPermanentWidget(m_language_label);
   m_ui->statusBar->addPermanentWidget(m_selected_count_label);
   m_ui->statusBar->addPermanentWidget(m_position_label);

   //------------------------------------------------------
   // restore size and position
   //------------------------------------------------------
/*
   QSettings settings;
   resize(settings.value(SETTINGS_SIZE, QSize(600, 600)).toSize());
   move(settings.value(SETTINGS_POS, QPoint(200, 200)).toPoint());
   restoreState(settings.value(SETTINGS_STATE).toByteArray());
   QDir::setCurrent(settings.value(SETTINGS_DIR).toString());

   m_ui->editor->setRecentFiles(settings.value(SETTINGS_RECENT).toStringList());
*/

   m_ui->splitter->setSizes(QList<int>() << 500 << 100);
   foreach (QString lang, m_ui->editor->languages()) {
      QAction* act = m_ui->languageMenu->addAction(lang);
      act->setCheckable(true);
   }

   // create initial empty file
   m_ui->editor->fileNew();
}
