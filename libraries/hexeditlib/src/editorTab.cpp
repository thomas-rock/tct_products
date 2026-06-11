#include "editorTab.h"
#include "editorWidget.h"
#include "preferencesDialog.h"
#include "summaryDialog.h"
#include <QtWidgets>
#include <QPrintDialog>

#define EDITOR(X) qobject_cast<editorWidget*>(X)

using namespace hexeditLib;

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
      text.replace("<TAB_COLOR>", "0000A0");
      setStyleSheet(text);
   }
   setTabsClosable(true);

   fileNew();
}
editorTab::~editorTab() {}
//-----------------------------------------------------------------------------
QStringList editorTab::recentFiles () const {return _recentFiles;}
void editorTab::setRecentFiles (const QStringList& list) {_recentFiles = list;}
QStringList editorTab::filenames () const
{
   QStringList filenames;
   foreach (editorWidget* ed, editors())
      filenames.append(ed->filename());

   return filenames;
}
//-----------------------------------------------------------------------------
QString editorTab::currentFilename () const {editorWidget* ed = editor(); return (ed) ? ed->filename() : QString();}
qint64 editorTab::currentAddress () const {editorWidget* ed = editor(); return (ed) ? ed->currentAddress() : 0;}
qint64 editorTab::currentSize () const {editorWidget* ed = editor(); return (ed) ? ed->currentSize() : 0;}
bool editorTab::currentMode () const {editorWidget* ed = editor(); return (ed) ? ed->overwriteMode() : true;}
bool editorTab::isUndoAvailable () const {editorWidget* ed = editor(); return (ed) ? ed->isUndoAvailable() : false;}
bool editorTab::isRedoAvailable () const {editorWidget* ed = editor(); return (ed) ? ed->isRedoAvailable() : false;}
bool editorTab::isModified () const {editorWidget* ed = editor(); return (ed) ? ed->isModified() : false;}
//-----------------------------------------------------------------------------
void editorTab::edit (const QString& filename, int line, int pos) {fileOpen(filename, line, pos);}
//-----------------------------------------------------------------------------
void editorTab::preferences ()
{
   preferencesDialog dialog(this);
   if (dialog.exec() == QDialog::Accepted) {
      dialog.savePreferences();
      updatePreferences();
   }
}
void editorTab::summary ()
{
   summaryDialog dialog(currentFilename(), this);
   dialog.exec();
}
//-----------------------------------------------------------------------------
void editorTab::fileNew () {createEditor(QString("[untitled-%1]").arg(_untitledCount++), false);}       // do not use pristine
void editorTab::fileOpen () {fileOpen(QFileDialog::getOpenFileNames(this, "Open File", QDir::currentPath(), "All Files (*.*)"));}
void editorTab::fileOpen (const QStringList& filenames)
{
   foreach (QString filename, filenames)
      fileOpen(filename);
}
bool editorTab::fileOpen (const QString& filename, int line, int pos)
{
   // check if file exists and select it if it does.
   editorWidget* ed = editor(filename);
   if (ed)
      setCurrentWidget(ed);

   // nope - new file. Create it
   else {
      QFileInfo fi(filename);
      if (fi.exists()) {
         QDir::setCurrent(fi.absolutePath());
         QString fn = fi.absoluteFilePath();

         ed = createEditor(fn);
         {
            QSignalBlocker tabBlock(ed);

            if (!ed->setData(filename, !fi.isWritable()))
               QMessageBox::critical(this, QString("Open Error"), QString("Cannot open file '%1' for reading").arg(filename));
            else {
               if (!_recentFiles.contains(filename))
                  _recentFiles.prepend(filename);
               if (_recentFiles.size() > 20) _recentFiles.pop_back();
            }
         }
      }

      else
         QMessageBox::critical(this, QString("Open Error"), QString("File '%1' does not exist").arg(filename));
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
      if (!ed->setData(filename, !fi.isWritable()))
         QMessageBox::critical(this, QString("Open Error"), QString("Cannot open file '%1' for reading").arg(filename));
   }
}
bool editorTab::fileSave(editorWidget* ed)
{
   if (ed == nullptr) ed = editor();
   QString filename = ed->filename();
   if (filename.startsWith('[')) return fileSaveAs();

   if (!ed->saveData()) {
      QMessageBox::critical(this, QString("File Error"), QString("Cannot open file '%1' for writing").arg(filename));
      return false;
   }
   return true;
}
bool editorTab::fileSaveAs ()
{
   editorWidget* ed = editor();
   QString oldFilename = ed->filename();

   QString filename = QFileDialog::getSaveFileName(this, QString("Save file"), (oldFilename.startsWith("[")) ? QDir::currentPath() : oldFilename, "All Files (*.*)");
   if (filename.isEmpty()) return false;     // canceled
   QFileInfo fi(filename);
   QDir::setCurrent(fi.absolutePath());

   ed->setFilename(filename);
   if (!ed->saveData())
      QMessageBox::critical(this, QString("File Error"), QString("Cannot open file '%1' for writing").arg(filename));
   else {
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
   editorWidget* ed = editor();

//   QsciPrinter printer;
//   QPrintDialog dialog(&printer, this);
//   if (dialog.exec() == QDialog::Accepted)
//      printer.printRange(ed);
}

// edit slots
//-----------------------------------------------------------------------------
void editorTab::undo ()  {editor()->undo();}
void editorTab::redo ()  {editor()->redo();}
void editorTab::cut ()   {/* editor()->cut(); */}
void editorTab::copy ()  {/* editor()->copy(); */}
void editorTab::del ()   {/* editor()->del(); */}
void editorTab::paste () {/* editor()->paste(); */}

// find/replace slots
//-----------------------------------------------------------------------------
void editorTab::find ()
{
//   _ui->_findReplaceAct->setChecked(true);
//   _ui->_findPanel->show();
}
void editorTab::find (QString text, bool hex, bool rev) {editor()->find(text, hex, rev);}
void editorTab::replace (QString ftext, bool fhex, QString rtext, bool rhex, bool rev) {editor()->replace(ftext, fhex, rtext, rhex, rev);}
void editorTab::replaceAll (QString ftext, bool fhex, QString rtext, bool rhex){editor()->replaceAll(ftext, fhex, rtext, rhex);}
void editorTab::gotoAddress ()
{
//   gotoDialog dialog(ed->lines());
//   if (dialog.exec() == QDialog::Accepted)
//   editor()->gotoLine(dialog.line());
}
void editorTab::tabSelected (QAction* action) {setCurrentWidget(editor(action->text()));}
//-----------------------------------------------------------------------------
void editorTab::closeTab (int index) {closeEditor(editors()[index]);}
void editorTab::modificationChanged ()
{
   editorWidget* ed = editor();
   tabBar()->setTabTextColor(indexOf(ed), (ed->isModified()) ? Qt::red : Qt::black);
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
      connect(ed,    SIGNAL(currentAddressChanged(qint64)), this,    SIGNAL(currentAddressChanged(qint64)));
      connect(ed,    SIGNAL(currentSizeChanged(qint64)),    this,    SIGNAL(currentSizeChanged(qint64)));
      connect(ed,    SIGNAL(overwriteModeChanged(bool)),    this,    SIGNAL(overwriteModeChanged(bool)));
      connect(ed,    SIGNAL(dataChanged()),                 this,    SIGNAL(dataChanged()));
      connect(ed,    SIGNAL(dataChanged()),                 this,    SLOT(modificationChanged()));
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
