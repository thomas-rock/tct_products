#pragma once

#include <QColor>
#include <QString>

struct WaveformCursor
{
   int            id      = 0;
   qreal          time    = 0.0;
   QColor         color   = Qt::yellow;
   bool           visible = true;
   bool           locked  = false;
   QString        name;

                  WaveformCursor() = default;

                  WaveformCursor(int id_,
                                 qreal time_,
                                 const QColor& color_ = Qt::yellow,
                                 bool visible_ = true,
                                 const QString& name_ = {},
                                 bool locked_ = false)
                     : id(id_),
                     time(time_),
                     color(color_),
                     visible(visible_),
                     locked(locked_),
                     name(name_)
                     {
                     }
};