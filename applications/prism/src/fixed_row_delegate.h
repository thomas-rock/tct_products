#pragma once

#include <QStyledItemDelegate>

class FixedRowDelegate : public QStyledItemDelegate
{
    Q_OBJECT

   public:
      explicit FixedRowDelegate(int rowHeight, QObject* parent = nullptr) : QStyledItemDelegate(parent), m_rowHeight(rowHeight) {}

      QSize sizeHint(const QStyleOptionViewItem& option,
                     const QModelIndex& index) const override
      {
         QSize s = QStyledItemDelegate::sizeHint(option, index);
         s.setHeight(m_rowHeight);
         return s;
      }

   private:
      int m_rowHeight;
};