#pragma once

#include "iwave_renderer.h"
#include "waveform_model.h"

#include <QBrush>
#include <QColor>
#include <QPen>
#include <QVector>

class BusRenderer : public IWaveRenderer
{
   public:
                           BusRenderer() = default;

      void                 setSegments(const QVector<WaveSegment>& segments);
      void                 setFieldSegments(const QVector<WaveSegment>& segments,
                                            int signalMsb,
                                            int signalLsb,
                                            int msb,
                                            int lsb);
      void                 setStyle(const WaveStyle& style);
      void                 setPixelsPerUnit(qreal pixelsPerUnit) override;

      void                 paint(QPainter* painter,
                                    const QRectF& rowRect,
                                    const QStyleOptionGraphicsItem* option) override;

      QString              valueAt(qreal x) const override;

   private:
      struct BusState
      {
         qreal startTime = 0.0;
         qreal endTime = 0.0;
         QString value;
      };

      QRectF               stateRect(const BusState& state, const QRectF& rowRect) const;
      QPolygonF            buildPolygon(const BusState& state, const QRectF& rowRect) const;
      QString              elidedText(const QFontMetricsF& fm, const BusState& state) const;
      bool                 intersectsExposed(const BusState& state,
                                             const QRectF& rowRect,
                                             const QRectF& exposed) const;

      bool                 containsUnknown(const QString& value) const;
      bool                 containsHighZ(const QString& value) const;

   private:
      QVector<BusState>    m_states;

      qreal                m_pixelsPerUnit = 10.0;

      QPen                 m_pen {QColor(0, 255, 0)};
      QBrush               m_brush {Qt::NoBrush};

      QColor               m_textColor {QColor(0, 255, 0)};
      QColor               m_xColor {QColor(255, 80, 80)};
      QColor               m_zColor {QColor(0, 255, 255)};

      qreal                m_slantWidth = 8.0;
      qreal                m_textMargin = 4.0;
};
