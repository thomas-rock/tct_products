#pragma once

#include <QString>

struct BusState
{
   qreal   x = 0.0;
   qreal   width = 0.0;
   QString value;

   BusState() = default;
   BusState(qreal x_, qreal width_, const QString& value_) : x(x_), width(width_), value(value_) {}

   qreal right() const { return x + width; }
};