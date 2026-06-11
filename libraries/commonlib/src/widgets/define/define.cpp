#include "define.h"
#include "define_dialog.h"
#include "ui_define.h"

#include <QMenu>
using namespace commonLib;

Define::Define (QWidget* parent) : QWidget(parent), m_ui(new Ui::Define), m_title(tr("Define"))
{
   m_ui->setupUi(this);

   m_ui->removeDefine->setEnabled(false);

   connect(m_ui->addDefine,    SIGNAL(clicked(bool)),                           this, SLOT(addDefine()));
   connect(m_ui->removeDefine, SIGNAL(clicked(bool)),                           this, SLOT(removeDefine()));
   connect(m_ui->defineTree,   SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(defineDoubleClicked(QTreeWidgetItem*)));
   connect(m_ui->defineTree,   SIGNAL(itemSelectionChanged()),                  this, SLOT(selectionChanged()));
}
void Define::clear () {m_ui->defineTree->clear();}
QMap<QString, QString> Define::defines () const
{
   QMap<QString, QString> defines;
   foreach (QTreeWidgetItem* item, topLevelItems())
      defines.insert(item->text(0), item->text(1));

   return defines;
}
void Define::addDefine (const QString& name, const QString& value)
{
   // check for duplicate and overwrite with new value
   foreach (QTreeWidgetItem* item, topLevelItems()) {
      if (item->text(0) == name) {
         item->setText(1, value);
         item->setToolTip(0, value);
         item->setToolTip(1, value);
         return;
      }
   }

   // add new item
   QTreeWidgetItem* item = new QTreeWidgetItem({name, value});
   item->setToolTip(0, value);
   item->setToolTip(1, value);
   m_ui->defineTree->addTopLevelItem(item);
}
void Define::removeDefine (const QString& name)
{
   foreach (QTreeWidgetItem* item, topLevelItems()) {
      if (item->text(0) == name) {
         delete item;
         m_ui->defineTree->clearSelection();
         return;
      }
   }
}
void Define::setTitle (const QString& title) {m_title = title;}

//-----------------------------------------------------------------------------
void Define::addDefine ()
{
   DefineDialog* dd = new DefineDialog(this, m_title);
   if (dd->exec()) {
      QPair<QString, QString> def = dd->define();
      addDefine(def.first, def.second);
   }
}
void Define::removeDefine ()
{
   foreach (QTreeWidgetItem* item, m_ui->defineTree->selectedItems())
      delete item;

   m_ui->defineTree->clearSelection();
}
//-----------------------------------------------------------------------------
void Define::defineDoubleClicked (QTreeWidgetItem* item) {emit defineDoubleClicked(item->text(0), item->text(1));}
void Define::selectionChanged () {m_ui->removeDefine->setEnabled(m_ui->defineTree->selectedItems().size() > 0);}
//-----------------------------------------------------------------------------
QList<QTreeWidgetItem*> Define::topLevelItems () const
{
   QList<QTreeWidgetItem*> items;
   for (int index = 0; index < m_ui->defineTree->topLevelItemCount(); index++)
      items.append(m_ui->defineTree->topLevelItem(index));
   return items;
}
