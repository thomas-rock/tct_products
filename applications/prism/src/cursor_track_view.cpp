#include "cursor_track_view.h"

#include <QWheelEvent>
#include <QFrame>

CursorTrackView::CursorTrackView(QWidget* parent) : QGraphicsView(parent)
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

void CursorTrackView::wheelEvent(QWheelEvent* event)
{
   event->ignore();
}