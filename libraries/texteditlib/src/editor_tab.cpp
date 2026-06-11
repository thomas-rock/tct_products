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
#include "editor_tab.h"
#include "editor_widget.h"
#include "lexer_manager.h"
#include "goto_dialog.h"
#include "preferences_dialog.h"
#include "summary_dialog.h"
#include "Qsci/qsciprinter.h"
#include "message.h"

// parsers
#include "json5_parser.h"
#include "parser.h"

#include <QtWidgets>
#include <QPrintDialog>

#define EDITOR(X) qobject_cast<EditorWidget*>(X)

using namespace texteditLib;
using namespace commonLib;

EditorTab::EditorTab (QWidget* parent) : QTabWidget(parent)
{
   // make connections
   connect(this,     SIGNAL(tabCloseRequested(int)),  this,    SLOT(closeTab(int)));
   connect(this,     SIGNAL(currentChanged(int)),     this,    SIGNAL(editorChanged()));

   // set styling
   QFile style(":/styles/tabWidget.txt");
   if (style.open(QIODevice::ReadOnly)) {
      QTextStream s(&style);
      QString text = s.readAll();
      text.replace("<TAB_COLOR>", "00A080");
      setStyleSheet(text);
   }
   setTabsClosable(true);

   QFontDatabase::addApplicationFont(":/textedit/SourceCodePro.ttf");
   LexerManager::initialize();
}
EditorTab::~EditorTab () {}

//-----------------------------------------------------------------------------
QStringList EditorTab::languages () const {return LexerManager::languages();}
QStringList EditorTab::recentFiles () const {return m_recent_files;}
void EditorTab::setRecentFiles (const QStringList& list) {m_recent_files = list;}
QStringList EditorTab::filenames () const
{
   QStringList filenames;
   foreach (EditorWidget* ed, editors())
      filenames.append(ed->filename());

   return filenames;
}
//-----------------------------------------------------------------------------
QString EditorTab::currentLanguage () const {EditorWidget* ed = editor(); return (ed) ? ed->language() : QString();}
QString EditorTab::currentFilename () const {EditorWidget* ed = editor(); return (ed) ? ed->filename() : QString();}
void EditorTab::cursorPosition (int* line, int* index) const {EditorWidget* ed = editor(); if (ed) ed->getCursorPosition(line, index);}
int EditorTab::currentLines () const {EditorWidget* ed = editor(); return (ed) ? ed->lines() : 0;}
bool EditorTab::isPristine () const {EditorWidget* ed = editor(); return (ed) ? ed->isPristine() : false;}
bool EditorTab::isUndoAvailable () const {EditorWidget* ed = editor(); return (ed) ? ed->isUndoAvailable() : false;}
bool EditorTab::isRedoAvailable () const {EditorWidget* ed = editor(); return (ed) ? ed->isRedoAvailable() : false;}
bool EditorTab::hasSelectedText () const {EditorWidget* ed = editor(); return (ed) ? ed->hasSelectedText() : false;}
int EditorTab::selectedCount () const {EditorWidget* ed = editor(); return (ed) ? ed->selectedCount() : 0;}
//-----------------------------------------------------------------------------
void EditorTab::edit (const QString& filename, int line, int pos) {fileOpen(filename, line, pos);}
void EditorTab::check ()
{
   EditorWidget* ed = editor();
   if (!ed) return;


   QString lang = editor()->language().toLower();
   if (lang.isEmpty()) return;

   else {
      MESSAGE(SysInfo, QString("Checking file %1").arg(ed->filename()));

      if (lang == "json") {
         QString json = ed->text();
         QTextStream jstream(&json);

         jsonLib::Json5Parser j5p(jstream);

         // forward messages
         connect(&j5p, &jsonLib::Json5Parser::message,
                 this, &EditorTab::message);

         j5p.parse();
      }
      else if (lang == "arti") {
         QString filename = ed->filename();

         artiLib::Parser p;
         connect(&p, &artiLib::Parser::message,
                 this, &EditorTab::message);

         connect(&p, SIGNAL(message(MessageType,QString,QString,int,int)), this, SLOT(handleMessage(MessageType, QString,QString,int,int)));
         p.parse(filename);
      }
      else
         MESSAGE(SysWarning, QString("Parser for language '%1' is not available. Cannot syntax check file").arg(lang));

      MESSAGE(SysInfo, QString("Syntax check complete"));
   }
}
//-----------------------------------------------------------------------------
void EditorTab::gotoLine ()
{
   GotoDialog dialog(currentLines());
   if (dialog.exec() == QDialog::Accepted) gotoLine(dialog.line());
}
void EditorTab::preferences ()
{
   PreferencesDialog dialog;
   if (dialog.exec() == QDialog::Accepted) {
      dialog.savePreferences();
      LexerManager::initialize();
      updatePreferences();
   }
}
void EditorTab::summary ()
{
   commonLib::SummaryDialog dialog(currentFilename());
   dialog.exec();
}
//-----------------------------------------------------------------------------
void EditorTab::fileNew () {createEditor(QString("[untitled-%1]").arg(m_untitled_count++), false);}       // do not use pristine
void EditorTab::fileOpen ()
{
   QString selectedFilter = "All Files (*.*)";
   fileOpen(QFileDialog::getOpenFileNames(this, "Open File", QDir::currentPath(), LexerManager::fileFilters(), &selectedFilter));
}
void EditorTab::fileOpen (const QStringList& filenames)
{
   foreach (QString filename, filenames)
      fileOpen(filename);
}
bool EditorTab::fileOpen (const QString& filename, int line, int pos)
{
   // check if file exists in any manager and select it if it does.
   EditorWidget* ed = editor(filename);
   if (ed != nullptr)
      setCurrentWidget(ed);

   // nope - new file. Create it
   else {
      QFileInfo fi(filename);
      if (fi.exists()) {
         QDir::setCurrent(fi.absolutePath());
         QString fn = fi.absoluteFilePath();
         QFile fio(fn);
         if (!fio.open(QIODevice::ReadOnly))
            QMessageBox::critical(this, QString("Open Error"), QString("Cannot open file '%1' for reading").arg(filename));

         else {
            QTextStream stream(&fio);
            ed = createEditor(fn);
            QString text = stream.readAll();
            ed->setText(text, !fi.isWritable());
         }
         if (!m_recent_files.contains(filename))
            m_recent_files.prepend(filename);
         if (m_recent_files.size() > 20) m_recent_files.pop_back();
      }

      else
         QMessageBox::critical(this, QString("Open Error"), QString("File '%1' does not exist").arg(filename));
   }

   // position cursor
   if (ed && line != -1) {
      if (pos != -1) ed->setCursorPosition(line, pos);
      else ed->gotoLine(line);
   }

   return ed != nullptr;
}
void EditorTab::fileCloseAll ()
{
   foreach (EditorWidget* ed, editors())
      closeEditor(ed);
}
void EditorTab::fileClose () {closeEditor(editor());}
void EditorTab::fileRevert ()
{
   EditorWidget* ed = editor();
   QString filename = ed->filename();
   if (filename.startsWith("[")) return;

   QFileInfo fi(filename);
   if (ed->isModified() && QMessageBox::question(this, QString("Verify reload"), QString("Okay to reload '%1' and lose all changes made?").arg(fi.fileName())) == QMessageBox::Yes) {
      filename = fi.absoluteFilePath();
      QFile fio(filename);
      if (!fio.open(QIODevice::ReadOnly))
         QMessageBox::critical(this, QString("Open Error"), QString("Cannot open file '%1' for reading").arg(filename));
      else {
         QTextStream stream(&fio);
         QString text = stream.readAll();
         ed->setText(text, !fi.isWritable());
         fio.close();
      }
   }
}
bool EditorTab::fileSave (EditorWidget* ed)
{
   if (ed == nullptr) ed = editor();
   QString filename = ed->filename();
   if (filename.startsWith('[')) return fileSaveAs();

   QFile fio(filename);
   if (!fio.open(QIODevice::WriteOnly)) {
      QString msg = QString("Cannot open file '%1' for writing").arg(filename);
      QMessageBox::critical(this, QString("File Error"), msg);
      MESSAGE(SysError, msg);
      return false;
   }
   QTextStream stream(&fio);
   stream << ed->text();
   fio.close();

   // update file status
   ed->fileSaved(filename);

   return true;
}

bool EditorTab::fileSaveAs ()
{
   EditorWidget* ed = editor();
   QString oldFilename = ed->filename();

   QString filters = LexerManager::fileFilters();
   QString filename = QFileDialog::getSaveFileName(this, QString("Save file"), (oldFilename.startsWith("[")) ? QDir::currentPath() : oldFilename, filters);
   if (filename.isEmpty()) return false;     // canceled
   QFileInfo fi(filename);
   QDir::setCurrent(fi.absolutePath());

   QFile fio(filename);
   if (!fio.open(QIODevice::WriteOnly))
      QMessageBox::critical(this, QString("File Error"), QString("Cannot open file '%1' for writing").arg(filename));

   else
   {
      QTextStream stream(&fio);
      stream << ed->text();
      fio.close();

      // update file status and tab text
      ed->fileSaved(filename);
      int index = indexOf(ed);
      setTabText(index, fi.fileName());
      setTabToolTip(index, filename);
   }
   return true;
}
void EditorTab::fileSaveAll ()
{
   foreach (EditorWidget* ed, editors())
      fileSave(ed);
}
void EditorTab::filePrint ()
{
   EditorWidget* ed = editor();

   QsciPrinter printer;
   QPrintDialog dialog(&printer, this);
   if (dialog.exec() == QDialog::Accepted)
      printer.printRange(ed);
}

// edit slots
//-----------------------------------------------------------------------------
void EditorTab::undo ()                  {editor()->undo();}
void EditorTab::redo ()                  {editor()->redo();}
void EditorTab::cut ()                   {editor()->cut();}
void EditorTab::copy ()                  {editor()->copy();}
void EditorTab::del ()                   {editor()->del();}
void EditorTab::paste ()                 {editor()->paste();}
void EditorTab::selectWord ()            {editor()->selectWord();}
void EditorTab::selectLine ()            {editor()->selectLine();}
void EditorTab::selectMatch ()           {editor()->selectToMatchingBrace();}
void EditorTab::selectAll ()             {editor()->selectAll();}
void EditorTab::indent ()                {editor()->indent();}
void EditorTab::unindent ()              {editor()->unindent();}
void EditorTab::uppercase ()             {editor()->uppercase();}
void EditorTab::lowercase ()             {editor()->lowercase();}
void EditorTab::capitalize ()            {editor()->capitalize();}
void EditorTab::changeCase ()            {editor()->changeCase();}
void EditorTab::comment ()               {editor()->comment();}
void EditorTab::uncomment ()             {editor()->uncomment();}
void EditorTab::windowsEol ()            {editor()->windowsEol();}
void EditorTab::unixEol ()               {editor()->unixEol();}
void EditorTab::macEol ()                {editor()->macEol();}
void EditorTab::trimTrailing ()          {editor()->trimTrailing();}
void EditorTab::trimLeading ()           {editor()->trimLeading();}
void EditorTab::trimLeadingTrailing ()   {editor()->trimLeadingTrailing();}
void EditorTab::tabs2Spaces ()           {editor()->tabs2Spaces();}
void EditorTab::spaces2Tabs ()           {editor()->spaces2Tabs();}
void EditorTab::setReadOnly ()           {editor()->setReadOnly();}
void EditorTab::clearReadOnly ()         {editor()->clearReadOnly();}
void EditorTab::copyPathname ()
{
   QString filename = editor()->filename();
   if (! filename.startsWith("[")) qApp->clipboard()->setText(filename);
}
void EditorTab::copyFilename ()
{
   QString filename = editor()->filename();
   if (!filename.startsWith("[")) qApp->clipboard()->setText(QFileInfo(filename).fileName());
}
void EditorTab::copyPath ()
{
   QString filename = editor()->filename();
   if (!filename.startsWith("[")) qApp->clipboard()->setText(QFileInfo(filename).path());
}
void EditorTab::copyAllPathnames ()
{
   QSet<QString> list;
   foreach (EditorWidget* ed, editors()) {
      QString filename = ed->filename();
      if (filename.startsWith("[")) continue;        // don't include untitled files
      list.insert(filename);
   }
   qApp->clipboard()->setText(list.values().join("\n"));
}
void EditorTab::copyAllFilenames ()
{
   QSet<QString> list;
   foreach (EditorWidget* ed, editors()) {
      QString filename = ed->filename();
      if (filename.startsWith("[")) continue;        // don't include untitled files
      list.insert(QFileInfo(filename).fileName());
   }
   qApp->clipboard()->setText(list.values().join("\n"));
}
void EditorTab::copyAllPaths ()
{
   QSet<QString> list;
   foreach (EditorWidget* ed, editors()) {
      QString filename = ed->filename();
      if (filename.startsWith("[")) continue;        // don't include untitled files
      list.insert(QFileInfo(filename).path());
   }
   qApp->clipboard()->setText(list.values().join("\n"));
}

// find/replace slots
//-----------------------------------------------------------------------------
void EditorTab::find (QString text, bool dir, bool cs, bool re, bool wo, bool wrap) {editor()->find(text, dir, cs, re, wo, wrap);}
void EditorTab::replace (QString text, QString newtext, bool all, bool next, bool dir, bool cs, bool re, bool wo, bool wrap)
{
   int count = editor()->replace(text, newtext, all, next, dir, cs, re, wo, wrap);
   emit replacedCount(count);
}
void EditorTab::gotoLine (int line) {editor()->gotoLine(line);}
void EditorTab::gotoMatch (bool select) {editor()->gotoMatch(select);}
void EditorTab::nextMarker () {editor()->nextMarker();}
void EditorTab::prevMarker () {editor()->prevMarker();}
void EditorTab::clearMarkers () {editor()->clearMarkers();}

// view slots
//-----------------------------------------------------------------------------
void EditorTab::lineNumbers (bool state) {editor()->lineNumbers(state);}
bool EditorTab::lineNumbers () {return editor()->lineNumbers();}
void EditorTab::markers (bool state)     {editor()->markers(state);}
bool EditorTab::markers ()     {return editor()->markers();}
void EditorTab::changeBars (bool state)  {editor()->changeBars(state);}
bool EditorTab::changeBars ()  {return editor()->changeBars();}
//void EditorTab::folding (bool state)     {editor()->folding(state);}
void EditorTab::indentation (bool state) {editor()->indentation(state);}
bool EditorTab::indentation () {return editor()->indentation();}
void EditorTab::whitespace (bool state)  {editor()->whitespace(state);}
bool EditorTab::whitespace ()  {return editor()->whitespace();}
void EditorTab::eols (bool state)        {editor()->eols(state);}
bool EditorTab::eols ()        {return editor()->eols();}
void EditorTab::highlight (bool state)   {editor()->highlight(state);}
bool EditorTab::highlight ()   {return editor()->highlight();}
void EditorTab::match (bool state)       {editor()->match(state);}
bool EditorTab::match ()       {return editor()->match();}
//void EditorTab::foldAll ()                         {editor()->foldAll();}
//void EditorTab::unfoldAll ()                       {editor()->unfoldAll();}
////    TODO: Add markers as unhide controls
//void EditorTab::hideSelectedLines ()               {editor()->hideSelectedLines();}

// language slots
//-----------------------------------------------------------------------------
void EditorTab::languageSelected (QAction* action) {editor()->setLanguage(action->text()); emit editorChanged();}
void EditorTab::tabSelected (QAction* action) {setCurrentWidget(editor(action->text()));}

// system slots
//-----------------------------------------------------------------------------
void EditorTab::closeTab (int index) {closeEditor(editors()[index]);}
void EditorTab::modificationChanged (EditorWidget* ed, bool state) {tabBar()->setTabTextColor(indexOf(ed), (state) ? Qt::red : Qt::black);}
void EditorTab::fileChanged (QString filename)
{
   EditorWidget* ed = editor(filename);
   if (ed == nullptr) return;    // failsafe

   if (QMessageBox::question(this, QString("File Changed"), QString("File '%1'changed outside of the editor. Update?").arg(filename)) == QMessageBox::Yes) {
      // reload file
      QFile fio(filename);
      if (!fio.open(QIODevice::ReadOnly)) {
         QString msg = QString("Cannot open file '%1' for reading").arg(filename);
         QMessageBox::critical(this, QString("Open Error"), msg);
         MESSAGE(SysError, msg);
      }
      else {
         QTextStream stream(&fio);
         QString text = stream.readAll();
         ed->setText(text, !QFileInfo(filename).isWritable());
         fio.close();
      }
   }
}

//-----------------------------------------------------------------------------
void EditorTab::updatePreferences ()
{
   foreach (EditorWidget* ed, editors())
      ed->updatePreferences();
}
EditorWidget* EditorTab::createEditor (QString filename, bool usePristine)
{
   EditorWidget* ed = nullptr;

   // search for pristine editor if enabled
   if (usePristine) {
      foreach (EditorWidget* e, editors())
         if (e->isPristine()) {
            ed = e;
             break;
         }
   }

   // create new editor
   if (!ed) {
      ed = new EditorWidget(this);

      // make connections
      connect(ed,    SIGNAL(cursorPositionChanged(int,int)), this,  SIGNAL(cursorPositionChanged(int,int)));
      connect(ed,    SIGNAL(selectionChanged()),             this,  SIGNAL(selectionChanged()));
      connect(ed,    SIGNAL(textChanged()),                  this,  SIGNAL(textChanged()));
      connect(ed,    SIGNAL(contextMenuRequest(QPoint)),     this,  SIGNAL(contextMenuRequest(QPoint)));
      connect(ed,    SIGNAL(selectedCount(int)),             this,  SIGNAL(selectedCount(int)));
      connect(ed,    SIGNAL(modificationChanged(EditorWidget*, bool)), this,  SLOT(modificationChanged(EditorWidget*, bool)));
   }

   int index = -1;
   {
      QSignalBlocker tabBlock(this);
      index = addTab(ed, QFileInfo(filename).fileName());
   }

   ed->setFilename(filename);
   setTabToolTip(index, filename);
   if (currentIndex() == index)
      emit currentChanged(index);
   else
      setCurrentIndex(index);

   return ed;
}

bool EditorTab::editorExists (QString filename) const {return editor(filename) != nullptr;}
EditorWidget* EditorTab::editor (QString filename) const
{
   if (filename.isEmpty()) return EDITOR(currentWidget());

   foreach (EditorWidget* ed, editors())
      if (ed->filename() == filename) return ed;

   return nullptr;
}

QList<EditorWidget*> EditorTab::editors() const
{
   QList<EditorWidget*> list;
   for (int index = 0; index < count(); index++)
      list.append(EDITOR(widget(index)));

   return list;
}

// closeEditor
void EditorTab::closeEditor (EditorWidget* ed)
{
   if (ed == nullptr) return;          // failsafe
   QString filename = ed->filename();
   if (editors().size() == 1 && filename.startsWith("[") && ed->isPristine()) return;     // don't close last unused editor

   if (ed->isModified()) {
      QMessageBox::StandardButton button = QMessageBox::question(this, QString("Verify Save"), QString("Save file %1 before closing?").arg(QFileInfo(filename).fileName()), QMessageBox::Yes| QMessageBox::No|QMessageBox::Cancel);
      if      (button == QMessageBox::Cancel) return;
      else if (button == QMessageBox::Yes && !fileSave()) return;    // file save(as) canceled
   }

   removeTab(indexOf(ed));
   delete ed;

   // always have one open editor
   if (count() == 0) fileNew();
}
