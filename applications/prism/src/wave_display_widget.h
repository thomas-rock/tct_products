#pragma once

#include "cursor_line_item.h"
#include "wave_graphics_view.h"
#include "wave_scene.h"
#include "waveform_cursor.h"
#include "waveform_model.h"

#include <QGraphicsRectItem>
#include <QHash>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVector>
#include <QWidget>

class WaveDisplayWidget : public QWidget
{
   Q_OBJECT

   public:
      explicit                WaveDisplayWidget(QWidget* parent = nullptr);

      void                    setWaveNameTree(QTreeWidget* tree);
      void                    setDisplayNodeMap(const QHash<QTreeWidgetItem*, DisplayNode*>* map);

      void                    setLogicalEndTime(qreal endTime);
      void                    setPixelsPerUnit(qreal pixelsPerUnit);

      qreal                   pixelsPerUnit() const { return m_pixelsPerUnit; }
      qreal                   scaleX(qreal time) const { return time * m_pixelsPerUnit; }
      qreal                   unscaleX(qreal x) const { return x / m_pixelsPerUnit; }

      void                    rebuildRows();
      void                    updateCursorLines(const QVector<WaveformCursor>& cursors);
      void                    setSelectedRowsFromTree();

      QTreeWidgetItem*        itemAtScenePos(const QPointF& scenePos) const;
      QScrollBar*             horizontalScrollBar();
      QScrollBar*             verticalScrollBar();
      QWidget*                viewport();

      void                    setQueryMatches(const QVector<WaveQueryEngine::WaveQueryMatch>& matches)
                              {
                                 m_scene->setQueryMatches(matches);
                              }

   signals:
      void                    zoomStepRequested(qreal zoomStep, qreal anchorSceneX);

      void                    leftMousePressed(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
      void                    leftMouseDragged(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
      void                    leftMouseReleased(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);

      void                    areaZoomRequested(qreal t0, qreal t1);

   protected:
      void                    resizeEvent(QResizeEvent* event) override;
      void                    showEvent(QShowEvent* event) override;

   private slots:
      void                    handleZoomStep(qreal step);
      void                    handleMiddlePressed(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
      void                    handleMiddleDragged(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
      void                    handleMiddleReleased(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);

   private:
      void                    rebuildSceneRect();
      void                    addTreeRowRecursive(QTreeWidgetItem* item, int& rowIndex);
      bool                    isItemVisibleByExpansion(QTreeWidgetItem* item) const;

      void                    ensureCursorItems(const QVector<WaveformCursor>& cursors);

      void                    beginAreaZoom(const QPointF& scenePos);
      void                    updateAreaZoom(const QPointF& scenePos);
      void                    finishAreaZoom(const QPointF& scenePos);
      void                    cancelAreaZoom();

   private:
      WaveGraphicsView*       m_view {nullptr};
      WaveScene*              m_scene {nullptr};

      QTreeWidget*            m_waveNameTree {nullptr};
      const QHash<QTreeWidgetItem*, DisplayNode*>* m_itemToDisplayNode {nullptr};

      QHash<int, CursorLineItem*> m_waveCursorItems;
      QVector<WaveformCursor> m_lastCursors;

      qreal                   m_pixelsPerUnit {10.0};
      qreal                   m_logicalEndTime {100.0};
      qreal                   m_currentSceneWaveWidth {1000.0};

      bool                    m_areaZooming = false;
      QPointF                 m_areaZoomStartScene;
      QGraphicsRectItem*      m_areaZoomRect = nullptr;

      static constexpr qreal WaveLeftMargin     = 0.0;
      static constexpr qreal TrackTopPadding    = 2.0;
      static constexpr qreal TrackBottomPadding = 3.0;
};