#include "bus_renderer.h"

#include "wave_value_utils.h"

#include <QFontMetricsF>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <algorithm>

void BusRenderer::setSegments(const QVector<WaveSegment>& segments)
{
   m_states.clear();
   m_states.reserve(segments.size());

   for (const auto& seg : segments)
   {
      m_states.push_back({
         seg.startTime,
         seg.endTime,
         seg.value
      });
   }
}

void BusRenderer::setFieldSegments(const QVector<WaveSegment>& segments,
                                   int signalMsb,
                                   int signalLsb,
                                   int msb,
                                   int lsb)
{
   m_states.clear();
   m_states.reserve(segments.size());

   for (const auto& seg : segments)
   {
      QString value;
      for (int bit = msb; bit >= lsb; --bit)
         value += WaveValueUtils::extractBusBit(seg.value, signalMsb, signalLsb, bit);

      m_states.push_back({
         seg.startTime,
         seg.endTime,
         value
      });
   }
}

void BusRenderer::setStyle(const WaveStyle& style)
{
   m_pen.setColor(style.normalColor);
   m_textColor = style.textColor;
   m_xColor = style.xColor;
   m_zColor = style.zColor;
}

void BusRenderer::setPixelsPerUnit(qreal pixelsPerUnit)
{
   m_pixelsPerUnit = pixelsPerUnit;
}

QRectF BusRenderer::stateRect(const BusState& state, const QRectF& rowRect) const
{
   const qreal x1 = state.startTime * m_pixelsPerUnit;
   const qreal x2 = state.endTime * m_pixelsPerUnit;

   return QRectF(x1, rowRect.y(), std::max<qreal>(0.0, x2 - x1), rowRect.height());
}

QPolygonF BusRenderer::buildPolygon(const BusState& state, const QRectF& rowRect) const
{
   const QRectF r = stateRect(state, rowRect);

   const qreal x1 = r.left();
   const qreal x2 = r.right();
   const qreal y1 = r.top();
   const qreal y2 = r.bottom();
   const qreal yc = r.center().y();

   const qreal slant = std::min<qreal>(m_slantWidth, r.width() * 0.5);

   QPolygonF poly;

   if (r.width() <= 2.0)
   {
      poly << QPointF(x1, y1)
      << QPointF(x2, y1)
      << QPointF(x2, y2)
      << QPointF(x1, y2);
      return poly;
   }

   poly << QPointF(x1, yc)
        << QPointF(x1 + slant, y1)
        << QPointF(x2 - slant, y1)
        << QPointF(x2, yc)
        << QPointF(x2 - slant, y2)
        << QPointF(x1 + slant, y2);

   return poly;
}

QString BusRenderer::elidedText(const QFontMetricsF& fm, const BusState& state) const
{
   const qreal width = (state.endTime - state.startTime) * m_pixelsPerUnit;
   const qreal available = width - 2.0 * m_textMargin;

   if (available <= 4.0)
      return {};

   return fm.elidedText(state.value, Qt::ElideRight, available);
}

bool BusRenderer::intersectsExposed(const BusState& state,
                                    const QRectF& rowRect,
                                    const QRectF& exposed) const
{
   return stateRect(state, rowRect).intersects(exposed);
}

bool BusRenderer::containsUnknown(const QString& value) const
{
   return value.contains('X', Qt::CaseInsensitive);
}

bool BusRenderer::containsHighZ(const QString& value) const
{
   return value.contains('Z', Qt::CaseInsensitive);
}

void BusRenderer::paint(QPainter* painter,
                        const QRectF& rowRect,
                        const QStyleOptionGraphicsItem* option)
{
   const QRectF exposed = option ? option->exposedRect : rowRect;
   const QFontMetricsF fm(painter->font());

   painter->setRenderHint(QPainter::Antialiasing, false);
   painter->setBrush(m_brush);

   for (const BusState& state : m_states)
   {
      if (state.endTime <= state.startTime)
         continue;

      if (!intersectsExposed(state, rowRect, exposed))
         continue;

      QPen pen = m_pen;
      QColor textColor = m_textColor;

      if (containsUnknown(state.value))
      {
         pen.setColor(m_xColor);
         textColor = m_xColor;
      }
      else if (containsHighZ(state.value))
      {
         pen.setColor(m_zColor);
         textColor = m_zColor;
      }

      painter->setPen(pen);
      painter->drawPolygon(buildPolygon(state, rowRect));

      const QString text = elidedText(fm, state);
      if (text.isEmpty())
         continue;

      const QRectF sr = stateRect(state, rowRect);
      const QRectF tr = fm.boundingRect(text);

      const qreal tx = sr.center().x() - tr.width() * 0.5;
      const qreal ty = sr.center().y() + (fm.ascent() - fm.descent()) * 0.5;

      painter->setPen(textColor);
      painter->drawText(QPointF(tx, ty), text);
   }
}

QString BusRenderer::valueAt(qreal x) const
{
   const qreal time = x / m_pixelsPerUnit;

   for (const BusState& state : m_states)
   {
      if (time >= state.startTime && time < state.endTime)
         return state.value;
   }

   return {};
}
