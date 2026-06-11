#include "signal_tree_widget.h"

#include <QHeaderView>
#include <QKeyEvent>
#include <QAbstractItemView>

SignalTreeWidget::SignalTreeWidget(QWidget* parent)
   : QTreeWidget(parent)
{
   setSelectionMode(QAbstractItemView::ExtendedSelection);
   setUniformRowHeights(true);
   setAlternatingRowColors(false);

   header()->setStretchLastSection(false);
   header()->setSectionResizeMode(NameColumn, QHeaderView::Stretch);

   if (columnCount() >= 2)
      header()->setSectionResizeMode(ValueColumn, QHeaderView::ResizeToContents);
   if (columnCount() >= 3)
      header()->setSectionResizeMode(RadixColumn, QHeaderView::ResizeToContents);
}

void SignalTreeWidget::keyPressEvent(QKeyEvent* event)
{
   if (event->key() == Qt::Key_Delete)
   {
      emit deletePressed();
      event->accept();
      return;
   }

   QTreeWidget::keyPressEvent(event);
}

void SignalTreeWidget::dropEvent(QDropEvent* event)
{
   if (event->mimeData()->hasFormat("application/x-wave-signal-paths"))
   {
      QTreeWidgetItem* target = itemAt(event->position().toPoint());

      emit externalMimeDropped(event->mimeData(), target, toTreeDropPosition(dropIndicatorPosition()));

      event->acceptProposedAction();
      return;
   }

   QTreeWidget::dropEvent(event);
   emit layoutMayHaveChanged();
}

QMimeData* SignalTreeWidget::mimeData(const QList<QTreeWidgetItem*>& items) const
{
   auto* mime = QTreeWidget::mimeData(items);

   QStringList ids;

   for (QTreeWidgetItem* item : items)
   {
      if (!item)
         continue;

      const QVariant v = item->data(0, Qt::UserRole);
      if (v.isValid())
         ids << v.toString();
   }

   if (!ids.isEmpty())
      mime->setData("application/x-wave-signal-paths", ids.join('\n').toUtf8());

   return mime;
}

SignalTreeWidget::TreeDropPosition SignalTreeWidget::toTreeDropPosition(QAbstractItemView::DropIndicatorPosition pos)
{
   switch (pos)
   {
      case QAbstractItemView::OnItem:
         return TreeDropPosition::OnItem;

      case QAbstractItemView::AboveItem:
         return TreeDropPosition::AboveItem;

      case QAbstractItemView::BelowItem:
         return TreeDropPosition::BelowItem;

      case QAbstractItemView::OnViewport:
      default:
         return TreeDropPosition::OnViewport;
   }
}