#include "cursor_track_widget.h"

#include <QVBoxLayout>
#include <algorithm>

CursorTrackWidget::CursorTrackWidget(QWidget* parent)
   : QWidget(parent)
{
   auto* layout = new QVBoxLayout(this);
   layout->setContentsMargins(0, 0, 0, 0);
   layout->setSpacing(0);

   m_scene = new CursorTrackScene(this);

   m_view = new CursorTrackView(this);
   m_view->setScene(m_scene);

   layout->addWidget(m_view);
}

void CursorTrackWidget::setLaneHeight(qreal height)
{
   m_laneHeight = height;
   m_scene->setLaneHeight(height);
}

void CursorTrackWidget::setSceneWidth(qreal width)
{
   m_scene->setTrackWidth(width);
   m_scene->setSceneRect(0.0, 0.0, width, m_scene->totalHeight());
}

void CursorTrackWidget::updateCursors(
   const QVector<WaveformCursor>& cursors,
   int activeCursorId,
   qreal pixelsPerUnit,
   const std::function<QString(qreal)>& formatTime,
   const std::function<QString(qreal)>& formatDelta)
{
   QVector<CursorTrackScene::CursorTrackInfo> info;

   const WaveformCursor* active = nullptr;
   for (const auto& c : cursors)
   {
      if (c.id == activeCursorId)
      {
         active = &c;
         break;
      }
   }

   for (const auto& cursor : cursors)
   {
      QString label = QString("%1 = %2").arg(cursor.name, formatTime(cursor.time));

      QString deltaLabel;
      if (active && cursor.id != active->id)
         deltaLabel = QString("dt=%1").arg(formatDelta(cursor.time - active->time));

      info.push_back({
         cursor.id,
         cursor.time * pixelsPerUnit,
         cursor.color,
         label,
         deltaLabel,
         cursor.visible,
         cursor.locked,
         cursor.id == activeCursorId
      });
   }

   m_scene->setCursors(info);

   int visibleCount = 0;
   for (const auto& c : cursors)
   {
      if (c.visible)
         ++visibleCount;
   }

   visibleCount = std::max(1, visibleCount);
   m_view->setFixedHeight(int(visibleCount * m_laneHeight));
}

QScrollBar* CursorTrackWidget::horizontalScrollBar()
{
   return m_view->horizontalScrollBar();
}

QWidget* CursorTrackWidget::viewport()
{
   return m_view->viewport();
}

int CursorTrackWidget::height()
{
   return m_view->height();
}

