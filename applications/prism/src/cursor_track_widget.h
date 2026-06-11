#pragma once

#include "cursor_track_scene.h"
#include "cursor_track_view.h"
#include "waveform_cursor.h"

#include <QWidget>

class CursorTrackWidget : public QWidget
{
   Q_OBJECT

   public:
      explicit             CursorTrackWidget(QWidget* parent = nullptr);

      void                 setLaneHeight(qreal height);
      void                 setSceneWidth(qreal width);
      void                 updateCursors(const QVector<WaveformCursor>& cursors,
                                         int activeCursorId,
                                         qreal pixelsPerUnit,
                                         const std::function<QString(qreal)>& formatTime,
                                         const std::function<QString(qreal)>& formatDelta);

      qreal                totalHeight() const { return m_scene->totalHeight(); }

      QScrollBar*          horizontalScrollBar();
      QWidget*             viewport();
      int                  height();

   private:
      CursorTrackView*     m_view {nullptr};
      CursorTrackScene*    m_scene {nullptr};
      qreal                m_laneHeight {20.0};
};