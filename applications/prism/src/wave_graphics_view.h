#pragma once

#include <QGraphicsView>
#include <Qt>

class WaveGraphicsView : public QGraphicsView
{
   Q_OBJECT
\
   public:
      explicit       WaveGraphicsView(QWidget* parent = nullptr);

   signals:
      void           zoomChanged(qreal zoomStep);
      void           mouseScenePositionChanged(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);

      void           leftMousePressed(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
      void           leftMouseDragged(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
      void           leftMouseReleased(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);

      void           middleMousePressed(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
      void           middleMouseDragged(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
      void           middleMouseReleased(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);

   protected:
      void           wheelEvent(QWheelEvent* event) override;
      void           mouseMoveEvent(QMouseEvent* event) override;
      void           mousePressEvent(QMouseEvent* event) override;
      void           mouseReleaseEvent(QMouseEvent* event) override;
};
