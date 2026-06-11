#pragma once

#include <QGraphicsView>

class TimeRulerView : public QGraphicsView
{
   Q_OBJECT

   public:
      explicit       TimeRulerView(QWidget* parent = nullptr);

   protected:
      void           wheelEvent(QWheelEvent* event) override;
};