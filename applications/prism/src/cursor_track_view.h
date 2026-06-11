#pragma once

#include <QGraphicsView>

class CursorTrackView : public QGraphicsView
{
   Q_OBJECT

   public:
      explicit       CursorTrackView(QWidget* parent = nullptr);

   protected:
      void           wheelEvent(QWheelEvent* event) override;
};