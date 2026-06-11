#include "time_ruler_view.h"

#include <QWheelEvent>

TimeRulerView::TimeRulerView(QWidget* parent)
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing, false);
    setDragMode(QGraphicsView::NoDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setResizeAnchor(QGraphicsView::NoAnchor);
    setInteractive(false);
}

void TimeRulerView::wheelEvent(QWheelEvent* event)
{
    event->ignore();
}