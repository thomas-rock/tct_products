#include "include_path.h"
#include "ui_include_path.h"
#include <QFileDialog>

using namespace commonLib;

IncludePath::IncludePath (QWidget* parent) : QWidget(parent), m_ui(new Ui::IncludePath), m_title(tr("Include Path"))
{
   m_ui->setupUi(this);

   m_ui->removePath->setEnabled(false);

   connect(m_ui->addPath,     SIGNAL(clicked(bool)),                           this, SLOT(addPath()));
   connect(m_ui->removePath,  SIGNAL(clicked(bool)),                           this, SLOT(removePath()));
   connect(m_ui->pathTree,    SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(pathDoubleClicked(QTreeWidgetItem*)));
   connect(m_ui->pathTree,    SIGNAL(itemSelectionChanged()),                  this, SLOT(selectionChanged()));
}
//-----------------------------------------------------------------------------
void IncludePath::clear () {m_ui->pathTree->clear();}
void IncludePath::addPath (const QString& path)
{
   QString apath = QDir(path).absolutePath();

   // check for duplicate
   foreach (QTreeWidgetItem* item, topLevelItems()) if (item->text(0) == apath)  return;    // ignore duplicates

   QTreeWidgetItem* item = new QTreeWidgetItem(QStringList{apath, apath});
   item->setToolTip(0, apath);
   m_ui->pathTree->addTopLevelItem(item);

   emit pathAdded(apath);
}
void IncludePath::removePath (const QString& path)
{
   QString apath = QDir(path).absolutePath();

   foreach (QTreeWidgetItem* item, topLevelItems()) {
      if (item->text(0) == apath) {
         delete item;
         m_ui->pathTree->clearSelection();
         emit pathRemoved(apath);
         break;
      }
   }
}
QStringList IncludePath::paths () const
{
   QStringList list;
   foreach (QTreeWidgetItem* item, topLevelItems())
      list.append(item->text(0));

   return list;
}
void IncludePath::setTitle (const QString& title) {m_title = title;}

//-----------------------------------------------------------------------------
void IncludePath::addPath ()
{
   QString path = QFileDialog::getExistingDirectory(this, m_title, QDir::currentPath());
   if (!path.isEmpty()) {
      QDir::setCurrent(path);
      addPath(path);
   }
}
void IncludePath::removePath ()
{
   foreach (QTreeWidgetItem* item, m_ui->pathTree->selectedItems()) {
      emit pathRemoved(item->text(1));
      delete item;
   }

   m_ui->pathTree->clearSelection();
}
//-----------------------------------------------------------------------------
void IncludePath::pathDoubleClicked (QTreeWidgetItem* item) {emit pathDoubleClicked(item->text(0));}
void IncludePath::selectionChanged () {m_ui->removePath->setEnabled(m_ui->pathTree->selectedItems().size() > 0);}
//-----------------------------------------------------------------------------
QList<QTreeWidgetItem*> IncludePath::topLevelItems () const
{
   QList<QTreeWidgetItem*> items;
   for (int index = 0; index < m_ui->pathTree->topLevelItemCount(); index++)
      items.append(m_ui->pathTree->topLevelItem(index));
   return items;
}
