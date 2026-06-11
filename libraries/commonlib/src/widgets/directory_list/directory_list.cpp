#include "directory_list.h"
#include <QDir>

using namespace commonLib;

DirectoryList::DirectoryList (QWidget* parent) : QTreeWidget(parent)
{
   setColumnCount(1);
   connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem*)));
}
//-----------------------------------------------------------------------------
void DirectoryList::addPath (const QString& path)
{
   if (m_watcher.directories().contains(path)) return;      // don't re-add
   m_watcher.addPath(path);

   // add top-level path
   QTreeWidgetItem* item = new QTreeWidgetItem(QStringList{path});
   addTopLevelItem(item);

   // add all subfiles
   foreach (QString file, QDir(path).entryList(QDir::Files))
      item->addChild(new QTreeWidgetItem(QStringList{file, QString("%1/%2").arg(path).arg(file)}));
}
void DirectoryList::removePath (const QString& path)
{
   m_watcher.removePath(path);
   for (int index = 0; index < topLevelItemCount(); index++) {
      QTreeWidgetItem* item = topLevelItem(index);
      if (item->text(0) == path) {
         delete item;
         break;
      }
   }
}
//-----------------------------------------------------------------------------
void DirectoryList::itemDoubleClicked (QTreeWidgetItem* item) {if (item->parent()) emit fileDoubleClicked(item->text(1));}
