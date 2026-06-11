#include "wave_track_item.h"

WaveTrackItem::WaveTrackItem(const QRectF& rowRect,
                             std::unique_ptr<IWaveRenderer> renderer,
                             QGraphicsItem* parent)
   : QGraphicsObject(parent),
   m_rowRect(0.0, 0.0, rowRect.width(), rowRect.height()),
   m_renderer(std::move(renderer))
{
   setPos(rowRect.topLeft());
   setAcceptedMouseButtons(Qt::NoButton);
   setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
}

QRectF WaveTrackItem::boundingRect() const
{
   constexpr qreal pad = 1.5;
   return m_rowRect.adjusted(-pad, -pad, pad, pad);
}

void WaveTrackItem::setRowRect(const QRectF& rect)
{
   prepareGeometryChange();
   setPos(rect.topLeft());
   m_rowRect = QRectF(0.0, 0.0, rect.width(), rect.height());
   update();
}

void WaveTrackItem::setSelectedRow(bool selected)
{
   if (m_selected == selected)
      return;

   m_selected = selected;
   update();
}

void WaveTrackItem::paint(QPainter* painter,
                          const QStyleOptionGraphicsItem* option,
                          QWidget* widget)
{
   Q_UNUSED(widget)

   if (m_selected)
      painter->fillRect(m_rowRect, QColor(60, 90, 140, 80));

   if (m_renderer)
      m_renderer->paint(painter, m_rowRect, option);
}

void WaveTrackItem::setPixelsPerUnit(qreal pixelsPerUnit)
{
   if (m_renderer)
      m_renderer->setPixelsPerUnit(pixelsPerUnit);

   update();
}