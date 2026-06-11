#include "viewerTab.h"
#include "viewerWidget.h"
//#include "preferencesDialog.h"
#include "summaryDialog.h"
#include <QtWidgets>
//#include <QPrintDialog>

#define VIEWER(X) qobject_cast<viewerWidget*>(X)

using namespace dotLib;

viewerTab::viewerTab (QWidget* parent) : QTabWidget(parent)
{
   // make connections
   connect(this,     SIGNAL(tabCloseRequested(int)),  this,    SLOT(closeTab(int)));
   connect(this,     SIGNAL(currentChanged(int)),     this,    SIGNAL(viewerChanged()));

   // set styling
   QFile style(":/styles/tabWidget.txt");
   if (style.open(QIODevice::ReadOnly)) {
      QTextStream s(&style);
      QString text = s.readAll();
      text.replace("<TAB_COLOR>", "A00000");    /* 00A080 */
      setStyleSheet(text);
   }
   setTabsClosable(true);
}
viewerTab::~viewerTab() {}
//-----------------------------------------------------------------------------
QStringList viewerTab::recentFiles () const {return _recentFiles;}
void viewerTab::setRecentFiles (const QStringList& list) {_recentFiles = list;}
QStringList viewerTab::filenames () const
{
   QStringList filenames;
   foreach (viewerWidget* v, viewers())
      filenames.append(v->filename());

   return filenames;
}
//-----------------------------------------------------------------------------
QString viewerTab::currentFilename () const {viewerWidget* ed = viewer(); return (ed) ? ed->filename() : QString();}
//-----------------------------------------------------------------------------
void viewerTab::view (const QString& filename) {fileOpen(filename);}
//-----------------------------------------------------------------------------
void viewerTab::preferences ()
{
/*
   preferencesDialog dialog(this);
   if (dialog.exec() == QDialog::Accepted) {
      dialog.savePreferences();
      updatePreferences();
   }
*/
}
void viewerTab::summary ()
{
   widgetLib::summaryDialog dialog(currentFilename(), this);
   dialog.exec();
}
//-----------------------------------------------------------------------------
void viewerTab::fileOpen () {fileOpen(QFileDialog::getOpenFileNames(this, "Open File", QDir::currentPath(), "All Files (*.*)"));}
void viewerTab::fileOpen (const QStringList& filenames)
{
   foreach (QString filename, filenames)
      fileOpen(filename);
}
bool viewerTab::fileOpen (const QString& filename)
{
   // check if file exists and select it if it does.
   viewerWidget* v = viewer(filename);
   if (v)
      setCurrentWidget(v);

   // nope - new file. Create it
   else {
      QFileInfo fi(filename);
      if (fi.exists()) {
         QDir::setCurrent(fi.absolutePath());
         QString fn = fi.absoluteFilePath();

         v = createViewer(fn);

         {
            QSignalBlocker tabBlock(v);

            QFile f(fn);
            if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
               QMessageBox::critical(this, QString("Open Error"), QString("Cannot open file '%1' for reading").arg(filename));\
            else {
               v->setDot(f.readAll());
               if (!_recentFiles.contains(fn))
                  _recentFiles.prepend(fn);
               if (_recentFiles.size() > 20) _recentFiles.pop_back();
            }
         }

      }

      else
         QMessageBox::critical(this, QString("Open Error"), QString("File '%1' does not exist").arg(filename));
   }

   return v != nullptr;
}
void viewerTab::fileCloseAll ()
{
   foreach (viewerWidget* v, viewers())
      closeViewer(v);
}
void viewerTab::fileClose () {closeViewer(viewer());}
void viewerTab::filePrint ()
{
   viewerWidget* v = viewer();

//   QsciPrinter printer;
//   QPrintDialog dialog(&printer, this);
//   if (dialog.exec() == QDialog::Accepted)
//      printer.printRange(ed);
}
//-----------------------------------------------------------------------------
void viewerTab::zoomIn ()  {viewer()->zoomIn();}
void viewerTab::zoomOut () {viewer()->zoomOut();}
void viewerTab::zoomFit () {viewer()->zoomFit();}
void viewerTab::zoom100 () {viewer()->zoom100();}
//-----------------------------------------------------------------------------
void viewerTab::closeTab (int index) {closeViewer(viewers()[index]);}
//-----------------------------------------------------------------------------
void viewerTab::updatePreferences ()
{
/*
   foreach (viewerWidget* v, viewers())
      v->updatePreferences();
*/
}
viewerWidget* viewerTab::createViewer (QString filename)
{
   viewerWidget* v = nullptr;

   // create new editor
   if (!v) {
      v = new viewerWidget();
   }

   int index = -1;
   {
      QSignalBlocker tabBlock(this);
      index = addTab(v, QFileInfo(filename).fileName());
   }

   v->setFilename(filename);
   setTabToolTip(index, filename);
   if (currentIndex() == index)
      emit currentChanged(index);
   else
      setCurrentIndex(index);

   return v;
}
bool viewerTab::viewerExists (QString filename) const {return viewer(filename) != nullptr;}
viewerWidget* viewerTab::viewer (QString filename) const
{
   if (filename.isEmpty()) return VIEWER(currentWidget());

   foreach (viewerWidget* v, viewers())
      if (v->filename() == filename) return v;

   return nullptr;
}
QList<viewerWidget*> viewerTab::viewers() const
{
   QList<viewerWidget*> list;
   for (int index = 0; index < count(); index++)
      list.append(VIEWER(widget(index)));

   return list;
}
void viewerTab::closeViewer (viewerWidget* v)
{
   if (v == nullptr) return;          // failsafe

   QString filename = v->filename();
   if (viewers().size() == 1 && filename.startsWith("[")) return;     // don't close last unused editor

   removeTab(indexOf(v));
   delete v;
}
