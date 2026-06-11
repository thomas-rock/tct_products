#include "cursor_line_item.h"

#include <QPainter>

CursorLineItem::CursorLineItem(qreal height,
                               const QColor& color,
                               QGraphicsItem* parent)
    : QGraphicsItem(parent),
      m_height(height),
      m_color(color)
{
    setZValue(10000.0);
}

QRectF CursorLineItem::boundingRect() const
{
   return QRectF(-1.0, 0.0, 2.0, m_height);
}

void CursorLineItem::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* option,
                           QWidget* widget)
{
   Q_UNUSED(option)
   Q_UNUSED(widget)

   QPen pen(m_color);
   pen.setWidthF(1.0);
   painter->setPen(pen);
   painter->drawLine(QPointF(0.0, 0.0), QPointF(0.0, m_height));
}

void CursorLineItem::setHeight(qreal height)
{
   if (qFuzzyCompare(m_height, height))
      return;

   m_height = height;
   prepareGeometryChange();
}

void CursorLineItem::setColor(const QColor& color)
{
    if (m_color == color)
        return;

    m_color = color;
    update();
}
