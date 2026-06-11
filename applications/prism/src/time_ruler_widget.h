#pragma once

#include "time_ruler_scene.h"
#include "time_ruler_view.h"
#include "waveform_cursor.h"

#include <QWidget>
#include <QVector>

class TimeRulerWidget : public QWidget
{
   Q_OBJECT

   public:
      explicit             TimeRulerWidget(QWidget* parent = nullptr);

      void                 setPixelsPerUnit(qreal pixelsPerUnit);
      void                 setSecondsPerUnit(qreal secondsPerUnit);
      void                 setAutoDisplayUnitEnabled(bool enabled);

      qreal                secondsPerUnit() const { return m_scene->secondsPerUnit(); }
      qreal                currentDisplayUnitSeconds() const { return m_scene->currentDisplayUnitSeconds(); }
      QString              currentDisplayUnitName() const { return m_scene->currentDisplayUnitName(); }
      qreal                majorTickStep() const { return m_scene->majorTickStep(); }

      void                 setSceneWidth(qreal width);
      void                 setHeaderHeight(int height);
      void                 updateCursorOverlay(const QVector<WaveformCursor>& cursors,
                                               qreal pixelsPerUnit);

      QScrollBar*          horizontalScrollBar();
      QWidget*             viewport();
      int                  height();

   private:
      TimeRulerView*       m_view {nullptr};
      TimeRulerScene*      m_scene {nullptr};
};