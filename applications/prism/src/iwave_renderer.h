#pragma once

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QRectF>

class IWaveRenderer
{
   public:
      virtual              ~IWaveRenderer() = default;

      virtual QString      valueAt(qreal x) const = 0;

      virtual void         paint(QPainter* painter,
                                 const QRectF& rowRect,
                                 const QStyleOptionGraphicsItem* option) = 0;

      virtual void         setPixelsPerUnit(qreal pixelsPerUnit) = 0;
};