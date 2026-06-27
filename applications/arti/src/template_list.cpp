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
#include "template_list.h"
#include <QDir>
#include <QMenu>

TemplateList::TemplateList (QWidget* parent) : QTreeWidget(parent), m_root{nullptr}, m_incSubdir{true}, m_expandNew{false}
{
   setColumnCount(1);

   // context menus
   m_templateContext = new QMenu;
   m_set_root = m_templateContext->addAction(QString("Set as root template"));
   m_templateContext->addSeparator();
   m_open = m_templateContext->addAction(QString("Open in editor"));

   connect(this, &QTreeWidget::itemDoubleClicked,
           this, &TemplateList::itemDoubleClicked);

   connect(&m_watcher, &QFileSystemWatcher::directoryChanged,
           this,       &TemplateList::directoryChanged);

   connect(this,     &QTreeWidget::customContextMenuRequested,
           this,     &TemplateList::contextMenu);

   connect(m_set_root,  &QAction::triggered,
           this,           &TemplateList::setCurrentRoot);

   connect(m_open,   &QAction::triggered,
           this,     &TemplateList::editCurrent);
}
//-----------------------------------------------------------------------------
QString TemplateList::currentPath ()
{
   QTreeWidgetItem* item = currentItem();
   if (!item) return QString();
   return item->text(1);
}
void TemplateList::setExpandNew (bool state) {m_expandNew = state;}
QString TemplateList::rootPath () {return (m_root) ? m_root->text(1) : QString();}
//-----------------------------------------------------------------------------
void TemplateList::addPath (const QString& path)
{
   if (m_watcher.directories().contains(path)) return;      // don't re-add
   m_watcher.addPath(path);

   // add top-level path
   QTreeWidgetItem* item = new QTreeWidgetItem(QStringList{path});
   addTopLevelItem(item);

   updatePath(path);

   item->setExpanded(m_expandNew);
}
void TemplateList::updatePath (const QString& path)
{
   // find QTreeWidgetItem associated with path
   for (int index = 0; index < topLevelItemCount(); index++) {
      QTreeWidgetItem* item = topLevelItem(index);
      if (item->text(0) == path) {
         updatePath(item);
         break;
      }
   }
}

void TemplateList::updatePath (QTreeWidgetItem* item)
{
   QString active;

   // delete each child. Check for active path
   while (item->childCount()) {
      QTreeWidgetItem* child = item->child(0);
      if (m_root && child->text(1) == m_root->text(1)) {
         active = child->text(1);
         m_root = nullptr;
         emit rootChanged(QString());
      }
      delete child;
   }

   // add items back in
   QString path = item->text(0);
   foreach (QString file, QDir(path).entryList(QDir::Files)) {
      if (isArtiFile(file))
         item->addChild(new QTreeWidgetItem(QStringList{file, QString("%1/%2").arg(path, file)}));
   }

   if (!active.isEmpty())
      setRootPath(active);
}

void TemplateList::removePath (const QString& path)
{
   m_watcher.removePath(path);
   for (int index = 0; index < topLevelItemCount(); index++) {
      QTreeWidgetItem* item = topLevelItem(index);
      if (item->text(0) == path) {
         for (int index = 0; index < item->childCount(); index++) {
            QTreeWidgetItem* child = item->child(index);
            if (child == m_root) {
               emit rootChanged(QString());
               m_root = nullptr;
            }
            delete child;
         }
         delete item;
         break;
      }
   }
}
void TemplateList::setCurrentRoot ()
{
   QTreeWidgetItem* item = currentItem();
   if (!item) return;

   clearRoot();

   QFont f = item->font(0);
   f.setBold(true);
   item->setFont(0, f);
   item->setForeground(0, QBrush(Qt::red, Qt::SolidPattern));
   m_root = item;

   emit rootChanged(currentPath());
}
void TemplateList::setRootPath (const QString& path)
{
   QTreeWidgetItem* item = findItem(path);
   if (!item) return;

   setCurrentItem(item);
   setCurrentRoot();
}
void TemplateList::editCurrent ()
{
   emit editRequest(currentPath());
}
void TemplateList::itemDoubleClicked (QTreeWidgetItem* item) {if (item->parent()) emit editRequest(item->text(1));}
void TemplateList::contextMenu ()
{
   QString path = currentPath();
   m_set_root->setEnabled(isArtiFile(path));
   m_templateContext->exec(QCursor::pos());
}

//-----------------------------------------------------------------------------
void TemplateList::directoryChanged (const QString& path)
{
   if (!QFileInfo::exists(path))
      removePath(path);
   else
      updatePath(path);
}
//-----------------------------------------------------------------------------
void TemplateList::clearRoot ()
{
   if (m_root) {
      m_root->setForeground(0, QBrush(Qt::black, Qt::SolidPattern));
      QFont f = m_root->font(0);
      f.setBold(false);
      m_root->setFont(0, f);
      m_root = nullptr;
      emit rootChanged(QString());
   }
}
QTreeWidgetItem* TemplateList::findItem (const QString& path)
{
   for (int index = 0; index < topLevelItemCount(); index++) {
      QTreeWidgetItem* top = topLevelItem(index);
      for(int cindex = 0; cindex < top->childCount(); cindex++) {
         QTreeWidgetItem* file = top->child(cindex);
         if (file->text(1) == path) return file;
      }
   }
   return nullptr;
}

bool TemplateList::isArtiFile (QString path)
{
   return QFileInfo(path).suffix().toLower() == "arti";
}

