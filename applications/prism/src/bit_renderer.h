#pragma once

#include "iwave_renderer.h"
#include "waveform_model.h"

#include <QColor>
#include <QPen>
#include <QVector>

class BitRenderer : public IWaveRenderer
{
   public:
                        BitRenderer() = default;

   void                 setSegments(const QVector<WaveSegment>& segments);
   void                 setBusBitSegments(const QVector<WaveSegment>& segments,
                                          int signalMsb,
                                          int signalLsb,
                                          int bitIndex);
   void                 setStyle(const WaveStyle& style);
   void                 setPixelsPerUnit(qreal pixelsPerUnit) override;

   void                 paint(QPainter* painter,
                                 const QRectF& rowRect,
                                 const QStyleOptionGraphicsItem* option) override;

   QString              valueAt(qreal x) const override;


   private:
      struct BitState
      {
         qreal startTime = 0.0;
         qreal endTime = 0.0;
         QString value;
      };

      QRectF            stateRect(const BitState& state, const QRectF& rowRect) const;
      bool              intersectsExposed(const BitState& state,
                                            const QRectF& rowRect,
                                            const QRectF& exposed) const;

      private:
      QVector<BitState> m_states;

      qreal             m_pixelsPerUnit = 10.0;

      QPen              m_pen {QColor(0, 255, 0)};
      QColor            m_textColor {QColor(0, 255, 0)};
      QColor            m_xColor {QColor(255, 80, 80)};
      QColor            m_zColor {QColor(0, 255, 255)};
};
