#pragma once

#include <QTreeWidget>
#include <QMimeData>

class SignalTreeWidget : public QTreeWidget
{
   Q_OBJECT

   public:
      enum Columns
      {
         NameColumn = 0,
         ValueColumn = 1,
         RadixColumn = 2
      };

      enum class TreeDropPosition
      {
         OnItem,
         AboveItem,
         BelowItem,
         OnViewport
      };

      explicit       SignalTreeWidget(QWidget* parent = nullptr);

   signals:
      void           layoutMayHaveChanged();
      void           deletePressed();
      void           externalMimeDropped(const QMimeData* mimeData,
                                         QTreeWidgetItem* targetItem,
                                         SignalTreeWidget::TreeDropPosition position);

   protected:
      void           keyPressEvent(QKeyEvent* event) override;
      void           dropEvent(QDropEvent* event) override;
      QMimeData*     mimeData(const QList<QTreeWidgetItem*>& items) const override;
      static         TreeDropPosition toTreeDropPosition(QAbstractItemView::DropIndicatorPosition pos);
};