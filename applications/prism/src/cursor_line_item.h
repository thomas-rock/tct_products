#pragma once

#include <QGraphicsItem>

class CursorLineItem : public QGraphicsItem
{
   public:
      explicit CursorLineItem(qreal height, const QColor& color = Qt::yellow, QGraphicsItem* parent = nullptr);

      QRectF   boundingRect() const override;
      void     paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

      void     setHeight(qreal height);

      void     setColor(const QColor& color);


      qreal    height() const { return m_height; }

      QColor   color() const { return m_color; }

   private:
      qreal    m_height{0.0};
      QColor   m_color{Qt::yellow};
};
