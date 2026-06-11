#include "wave_graphics_view.h"

#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QFrame>

WaveGraphicsView::WaveGraphicsView(QWidget* parent)
   : QGraphicsView(parent)
{
   setRenderHint(QPainter::Antialiasing, false);
   setDragMode(QGraphicsView::NoDrag);
   setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
   setResizeAnchor(QGraphicsView::NoAnchor);
   setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

   setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   setFrameShape(QFrame::NoFrame);
   setAlignment(Qt::AlignLeft | Qt::AlignTop);

   setMouseTracking(true);
   viewport()->setMouseTracking(true);
}

void WaveGraphicsView::wheelEvent(QWheelEvent* event)
{
   if (event->modifiers() & Qt::ControlModifier)
   {
      const qreal step = (event->angleDelta().y() > 0) ? 1.15 : (1.0 / 1.15);
      emit zoomChanged(step);
      event->accept();
      return;
   }

   QGraphicsView::wheelEvent(event);
}

void WaveGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
   const QPointF scenePos = mapToScene(event->pos());

   emit mouseScenePositionChanged(scenePos, event->modifiers());

   if (event->buttons() & Qt::LeftButton)
      emit leftMouseDragged(scenePos, event->modifiers());

   if (event->buttons() & Qt::MiddleButton)
      emit middleMouseDragged(scenePos, event->modifiers());

   QGraphicsView::mouseMoveEvent(event);
}

void WaveGraphicsView::mousePressEvent(QMouseEvent* event)
{
   const QPointF scenePos = mapToScene(event->pos());

   if (event->button() == Qt::LeftButton)
   {
      emit leftMousePressed(scenePos, event->modifiers());
      event->accept();
      return;
   }

   if (event->button() == Qt::MiddleButton)
   {
      emit middleMousePressed(scenePos, event->modifiers());
      event->accept();
      return;
   }

   QGraphicsView::mousePressEvent(event);
}

void WaveGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
   const QPointF scenePos = mapToScene(event->pos());

   if (event->button() == Qt::LeftButton)
   {
      emit leftMouseReleased(scenePos, event->modifiers());
      event->accept();
      return;
   }

   if (event->button() == Qt::MiddleButton)
   {
      emit middleMouseReleased(scenePos, event->modifiers());
      event->accept();
      return;
   }

   QGraphicsView::mouseReleaseEvent(event);
}
