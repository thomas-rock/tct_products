#include "time_ruler_widget.h"

#include <QVBoxLayout>

TimeRulerWidget::TimeRulerWidget(QWidget* parent)
   : QWidget(parent)
{
   auto* layout = new QVBoxLayout(this);
   layout->setContentsMargins(0, 0, 0, 0);
   layout->setSpacing(0);

   m_scene = new TimeRulerScene(this);

   m_view = new TimeRulerView(this);
   m_view->setScene(m_scene);

   layout->addWidget(m_view);
}

void TimeRulerWidget::setPixelsPerUnit(qreal pixelsPerUnit)
{
   m_scene->setPixelsPerUnit(pixelsPerUnit);
}

void TimeRulerWidget::setSecondsPerUnit(qreal secondsPerUnit)
{
   m_scene->setSecondsPerUnit(secondsPerUnit);
}

void TimeRulerWidget::setAutoDisplayUnitEnabled(bool enabled)
{
   m_scene->setAutoDisplayUnitEnabled(enabled);
}

void TimeRulerWidget::setSceneWidth(qreal width)
{
   m_scene->setSceneRect(0.0, 0.0, width, m_scene->rulerHeight());
}

void TimeRulerWidget::setHeaderHeight(int height)
{
   m_scene->setRulerHeight(height);
   m_view->setFixedHeight(height);
}

void TimeRulerWidget::updateCursorOverlay(const QVector<WaveformCursor>& cursors,
                                          qreal pixelsPerUnit)
{
   QVector<TimeRulerScene::RulerCursorInfo> info;

   for (const auto& cursor : cursors)
   {
      info.push_back({
         cursor.time * pixelsPerUnit,
         cursor.color,
         cursor.name,
         cursor.visible
      });
   }

   m_scene->setCursors(info);
}

QScrollBar* TimeRulerWidget::horizontalScrollBar()
{
   return m_view->horizontalScrollBar();
}

QWidget* TimeRulerWidget::viewport()
{
   return m_view->viewport();
}

int TimeRulerWidget::height()
{
   return m_view->height();
}

