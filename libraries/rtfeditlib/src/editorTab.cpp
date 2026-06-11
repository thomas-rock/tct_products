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
#include "editorTab.h"
#include "editorWidget.h"
#include "summaryDialog.h"
#include "message.h"

#include <QtWidgets>
#include <QPrinter>
#include <QPrintDialog>

#define EDITOR(X) qobject_cast<editorWidget*>(X)

using namespace rtfeditLib;

editorTab::editorTab (QWidget* parent) : QTabWidget(parent)
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
}
editorTab::~editorTab () {}
//-----------------------------------------------------------------------------
QStringList editorTab::recentFiles () const {return _recentFiles;}
void editorTab::setRecentFiles (const QStringList& list) {_recentFiles = list;}
QStringList editorTab::filenames ()
{
   QStringList filenames;
   foreach (editorWidget* ed, editors())
      filenames.append(ed->filename());

   return filenames;
}
//-----------------------------------------------------------------------------
QString editorTab::currentFilename () const {editorWidget* ed = editor(); return (ed) ? ed->filename() : QString();}
void editorTab::cursorPosition (int* line, int* index) const {editorWidget* ed = editor(); if (ed) ed->getCursorPosition(line, index);}
int editorTab::currentLines () const {editorWidget* ed = editor(); return (ed) ? ed->lines() : 0;}
bool editorTab::isPristine () const {editorWidget* ed = editor(); return (ed) ? ed->isPristine() : false;}
bool editorTab::isUndoAvailable () const {editorWidget* ed = editor(); return (ed) ? ed->isUndoAvailable() : false;}
bool editorTab::isRedoAvailable () const {editorWidget* ed = editor(); return (ed) ? ed->isRedoAvailable() : false;}
bool editorTab::hasSelectedText () const {editorWidget* ed = editor(); return (ed) ? ed->hasSelectedText() : false;}
int editorTab::selectedCount () const {editorWidget* ed = editor(); return (ed) ? ed->selectedCount() : 0;}
//-----------------------------------------------------------------------------
void editorTab::edit (const QString& filename, int line, int pos) {fileOpen(filename, line, pos);}
//-----------------------------------------------------------------------------
void editorTab::preferences ()
{
/*
   preferencesDialog dialog;
   if (dialog.exec() == QDialog::Accepted) {
      dialog.savePreferences();
      updatePreferences();
   }
*/
}
void editorTab::summary ()
{
   widgetLib::summaryDialog dialog(currentFilename());
   dialog.exec();
}
//-----------------------------------------------------------------------------
void editorTab::fileNew () {qDebug() << "editorTab::fileNew"; createEditor(QString("[untitled-%1]").arg(_untitledCount++), false);}       // do not use pristine
void editorTab::fileOpen ()
{
   QString selectedFilter = "All Files (*.*)";
   fileOpen(QFileDialog::getOpenFileNames(this, "Open File", QDir::currentPath(), selectedFilter, &selectedFilter));
}
void editorTab::fileOpen (const QStringList& filenames)
{
   foreach (QString filename, filenames)
      fileOpen(filename);
}
bool editorTab::fileOpen (const QString& filename, int line, int pos)
{
   // check if file exists in any manager and select it if it does.
   editorWidget* ed = editor(filename);
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
         if (!_recentFiles.contains(filename))
            _recentFiles.prepend(filename);
         if (_recentFiles.size() > 20) _recentFiles.pop_back();
      }

      else
         QMessageBox::critical(this, QString("Open Error"), QString("File '%1' does not exist").arg(filename));
   }

   // position cursor
   if (ed && line != -1) {
      if (pos != -1) ed->setCursorPosition(line, pos);
      else ed->setCursorPosition(line, 0);
   }

   return ed != nullptr;
}
void editorTab::fileCloseAll ()
{
   foreach (editorWidget* ed, editors())
      closeEditor(ed);
}
void editorTab::fileClose () {closeEditor(editor());}
void editorTab::fileRevert ()
{
   editorWidget* ed = editor();
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
bool editorTab::fileSave (editorWidget* ed)
{
   if (ed == nullptr) ed = editor();
   QString filename = ed->filename();
   if (filename.startsWith('[')) return fileSaveAs();

   QFile fio(filename);
   if (!fio.open(QIODevice::WriteOnly)) {
      QString msg = QString("Cannot open file '%1' for writing").arg(filename);
      QMessageBox::critical(this, QString("File Error"), msg);
      MESSAGE(ErrorMessage, msg);
      return false;
   }
   QTextStream stream(&fio);
//   stream << ed->text();
   fio.close();

   // update file status
   ed->fileSaved(filename);

   return true;
}

bool editorTab::fileSaveAs ()
{
   editorWidget* ed = editor();
   QString oldFilename = ed->filename();

   QString filename = QFileDialog::getSaveFileName(this, QString("Save file"), (oldFilename.startsWith("[")) ? QDir::currentPath() : oldFilename);
   if (filename.isEmpty()) return false;     // canceled
   QFileInfo fi(filename);
   QDir::setCurrent(fi.absolutePath());

   QFile fio(filename);
   if (!fio.open(QIODevice::WriteOnly))
      QMessageBox::critical(this, QString("File Error"), QString("Cannot open file '%1' for writing").arg(filename));

   else
   {
      QTextStream stream(&fio);
//      stream << ed->text();
      fio.close();

      // update file status and tab text
      ed->fileSaved(filename);
      int index = indexOf(ed);
      setTabText(index, fi.fileName());
      setTabToolTip(index, filename);
   }
   return true;
}
void editorTab::fileSaveAll ()
{
   foreach (editorWidget* ed, editors())
      fileSave(ed);
}
void editorTab::filePrint ()
{
   QPrinter printer;
   QPrintDialog dialog(&printer, this);
   if (dialog.exec() == QDialog::Accepted)
      editor()->print(&printer);
}

//-----------------------------------------------------------------------------
void editorTab::undo ()       {editor()->undo();}
void editorTab::redo ()       {editor()->redo();}
void editorTab::cut ()        {editor()->cut();}
void editorTab::copy ()       {editor()->copy();}
void editorTab::paste ()      {editor()->paste();}
void editorTab::del ()        {editor()->del();}
void editorTab::selectWord () {editor()->selectWord();}
void editorTab::selectLine () {editor()->selectLine();}
void editorTab::selectAll ()  {editor()->selectAll();}
//-----------------------------------------------------------------------------
void editorTab::indent ()        {editor()->indent();}
void editorTab::unindent ()      {editor()->unindent();}
void editorTab::uppercase ()     {editor()->uppercase();}
void editorTab::lowercase ()     {editor()->lowercase();}
void editorTab::capitalize ()    {editor()->capitalize();}
void editorTab::changeCase ()    {editor()->changeCase();}
void editorTab::tabs2Spaces ()   {editor()->tabs2Spaces();}
void editorTab::spaces2Tabs ()   {editor()->spaces2Tabs();}
void editorTab::setReadOnly ()   {editor()->setReadOnly(true);}
void editorTab::clearReadOnly () {editor()->setReadOnly(false);}
void editorTab::copyPathname ()
{
   QString filename = editor()->filename();
   if (! filename.startsWith("[")) qApp->clipboard()->setText(filename);
}
void editorTab::copyFilename ()
{
   QString filename = editor()->filename();
   if (!filename.startsWith("[")) qApp->clipboard()->setText(QFileInfo(filename).fileName());
}
void editorTab::copyPath ()
{
   QString filename = editor()->filename();
   if (!filename.startsWith("[")) qApp->clipboard()->setText(QFileInfo(filename).path());
}
void editorTab::copyAllPathnames ()
{
   QSet<QString> list;
   foreach (editorWidget* ed, editors()) {
      QString filename = ed->filename();
      if (filename.startsWith("[")) continue;        // don't include untitled files
      list.insert(filename);
   }
   qApp->clipboard()->setText(list.values().join("\n"));
}
void editorTab::copyAllFilenames ()
{
   QSet<QString> list;
   foreach (editorWidget* ed, editors()) {
      QString filename = ed->filename();
      if (filename.startsWith("[")) continue;        // don't include untitled files
      list.insert(QFileInfo(filename).fileName());
   }
   qApp->clipboard()->setText(list.values().join("\n"));
}
void editorTab::copyAllPaths ()
{
   QSet<QString> list;
   foreach (editorWidget* ed, editors()) {
      QString filename = ed->filename();
      if (filename.startsWith("[")) continue;        // don't include untitled files
      list.insert(QFileInfo(filename).path());
   }
   qApp->clipboard()->setText(list.values().join("\n"));
}
//-----------------------------------------------------------------------------
void editorTab::find (QString text, bool dir, bool cs, bool re, bool wo, bool wrap) {editor()->find(text, dir, cs, re, wo, wrap);}
void editorTab::replace (QString text, QString newtext, bool all, bool next, bool dir, bool cs, bool re, bool wo, bool wrap) {editor()->replace(text, newtext, all, next, dir, cs, re, wo, wrap);}
//-----------------------------------------------------------------------------
void editorTab::setStatus (bool state) {/*_ui->_statusPanel->setVisible(state);*/}
//-----------------------------------------------------------------------------
void editorTab::closeTab (int index) {closeEditor(editors()[index]);}
void editorTab::modificationChanged (editorWidget* ed, bool state) {tabBar()->setTabTextColor(indexOf(ed), (state) ? Qt::red : Qt::black);}
void editorTab::fileChanged (QString filename)
{
   editorWidget* ed = editor(filename);
   if (ed == nullptr) return;    // failsafe

   if (QMessageBox::question(this, QString("File Changed"), QString("File '%1'changed outside of the editor. Update?").arg(filename)) == QMessageBox::Yes) {
      // reload file
      QFile fio(filename);
      if (!fio.open(QIODevice::ReadOnly)) {
         QString msg = QString("Cannot open file '%1' for reading").arg(filename);
         QMessageBox::critical(this, QString("Open Error"), msg);
         MESSAGE(ErrorMessage, msg);
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
void editorTab::updatePreferences ()
{
   foreach (editorWidget* ed, editors())
      ed->updatePreferences();
}
editorWidget* editorTab::createEditor (QString filename, bool usePristine)
{
   editorWidget* ed = nullptr;

   // search for pristine editor if enabled
   if (usePristine) {
      foreach (editorWidget* e, editors())
         if (e->isPristine()) {
            ed = e;
             break;
         }
   }

   // create new editor
   if (!ed) {
      ed = new editorWidget(this);

      // make connections
      connect(ed,    SIGNAL(cursorPositionChanged(int,int)), this,  SIGNAL(cursorPositionChanged(int,int)));
      connect(ed,    SIGNAL(selectionChanged()),             this,  SIGNAL(selectionChanged()));
      connect(ed,    SIGNAL(textChanged()),                  this,  SIGNAL(textChanged()));
      connect(ed,    SIGNAL(contextMenuRequest(QPoint)),     this,  SIGNAL(contextMenuRequest(QPoint)));
      connect(ed,    SIGNAL(selectedCount(int)),             this,  SIGNAL(selectedCount(int)));
      connect(ed,    SIGNAL(modificationChanged(editorWidget*, bool)), this,  SLOT(modificationChanged(editorWidget*, bool)));
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

bool editorTab::editorExists (QString filename) const {return editor(filename) != nullptr;}
editorWidget* editorTab::editor (QString filename) const
{
   if (filename.isEmpty()) return EDITOR(currentWidget());

   foreach (editorWidget* ed, editors())
      if (ed->filename() == filename) return ed;

   return nullptr;
}

QList<editorWidget*> editorTab::editors() const
{
   QList<editorWidget*> list;
   for (int index = 0; index < count(); index++)
      list.append(EDITOR(widget(index)));

   return list;
}

// closeEditor
void editorTab::closeEditor (editorWidget* ed)
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

