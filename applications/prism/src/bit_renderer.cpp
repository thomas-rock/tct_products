#include "bit_renderer.h"

#include "wave_value_utils.h"

#include <QFontMetricsF>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <algorithm>

void BitRenderer::setSegments(const QVector<WaveSegment>& segments)
{
   m_states.clear();
   m_states.reserve(segments.size());

   for (const auto& seg : segments)
   {
      m_states.push_back({
         seg.startTime,
         seg.endTime,
         WaveValueUtils::normalizeBitValue(seg.value)
      });
   }
}

void BitRenderer::setBusBitSegments(const QVector<WaveSegment>& segments, int bitIndex)
{
   m_states.clear();
   m_states.reserve(segments.size());

   for (const auto& seg : segments)
   {
      m_states.push_back({
         seg.startTime,
         seg.endTime,
         WaveValueUtils::extractBusBit(seg.value, bitIndex)
      });
   }
}

void BitRenderer::setStyle(const WaveStyle& style)
{
   m_pen.setColor(style.normalColor);
   m_textColor = style.textColor;
   m_xColor = style.xColor;
   m_zColor = style.zColor;
}

void BitRenderer::setPixelsPerUnit(qreal pixelsPerUnit)
{
   m_pixelsPerUnit = pixelsPerUnit;
}

QRectF BitRenderer::stateRect(const BitState& state, const QRectF& rowRect) const
{
   const qreal x1 = state.startTime * m_pixelsPerUnit;
   const qreal x2 = state.endTime * m_pixelsPerUnit;

   return QRectF(x1,
                 rowRect.y(),
                 std::max<qreal>(0.0, x2 - x1),
                 rowRect.height());
}

bool BitRenderer::intersectsExposed(const BitState& state,
                                    const QRectF& rowRect,
                                    const QRectF& exposed) const
{
   return stateRect(state, rowRect).intersects(exposed);
}

void BitRenderer::paint(QPainter* painter,
                        const QRectF& rowRect,
                        const QStyleOptionGraphicsItem* option)
{
   const QRectF exposed = option ? option->exposedRect : rowRect;

   painter->setRenderHint(QPainter::Antialiasing, false);

   const qreal highY = rowRect.top() + rowRect.height() * 0.20;
   const qreal lowY  = rowRect.top() + rowRect.height() * 0.80;
   const qreal midY  = rowRect.center().y();

   for (int i = 0; i < m_states.size(); ++i)
   {
      const BitState& state = m_states[i];

      if (state.endTime <= state.startTime)
         continue;

      if (!intersectsExposed(state, rowRect, exposed))
         continue;

      QRectF r = stateRect(state, rowRect);
      if (r.width() <= 0.0)
         continue;

      QPen pen = m_pen;
      QColor textColor = m_textColor;

      const QString v = state.value.toUpper();

      if (v == "X")
      {
         pen.setColor(m_xColor);
         textColor = m_xColor;
      }
      else if (v == "Z")
      {
         pen.setColor(m_zColor);
         textColor = m_zColor;
      }

      painter->setPen(pen);

      qreal y = midY;
      if (v == "1")
         y = highY;
      else if (v == "0")
         y = lowY;

      if (v == "0" || v == "1")
      {
         painter->drawLine(QPointF(r.left(), y), QPointF(r.right(), y));

         if (i > 0)
         {
            const BitState& prev = m_states[i - 1];
            const QString pv = prev.value.toUpper();

            if ((pv == "0" || pv == "1") && pv != v)
            {
               const qreal prevY = (pv == "1") ? highY : lowY;
               painter->drawLine(QPointF(r.left(), prevY), QPointF(r.left(), y));
            }
         }
      }
      else
      {
         painter->drawLine(QPointF(r.left(), midY), QPointF(r.right(), midY));

         const QString label = v;
         const QFontMetricsF fm(painter->font());
         const QRectF textRect = fm.boundingRect(label);

         const qreal tx = r.center().x() - textRect.width() * 0.5;
         const qreal ty = midY + (fm.ascent() - fm.descent()) * 0.5;

         painter->setPen(textColor);
         painter->drawText(QPointF(tx, ty), label);
      }
   }
}

QString BitRenderer::valueAt(qreal x) const
{
   const qreal time = x / m_pixelsPerUnit;

   for (const BitState& state : m_states)
   {
      if (time >= state.startTime && time < state.endTime)
         return state.value;
   }

   return {};
}