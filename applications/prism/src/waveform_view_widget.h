#pragma once

#include "cursor_track_widget.h"
#include "signal_tree_widget.h"
#include "time_ruler_widget.h"
#include "wave_display_widget.h"
#include "waveform_cursor.h"
#include "waveform_model.h"

#include <QHash>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>
#include <QMimeData>
#include <QAbstractItemView>
#include <QJsonArray>

#include <memory>
#include <vector>

class QTreeWidgetItem;

class WaveformViewWidget : public QWidget
{
   Q_OBJECT

   public:
      explicit                WaveformViewWidget(QWidget* parent = nullptr);

      void                    populateDemoData();
      void                    loadDocument(std::unique_ptr<WaveformDocument> document);

      void                    zoomFit();
      void                    zoomBetweenCursors();
      void                    zoomToTimeRange(qreal t0, qreal t1);

      QJsonArray              waveLayoutToJson() const;
      bool                    loadWaveLayoutFromJson(const QJsonArray& waves);

   private slots:
      void                    rebuildWaveRows();

      void                    syncTreeToWaveScroll(int value);
      void                    syncWaveToTreeScroll(int value);
      void                    syncWaveToRulerScroll(int value);
      void                    syncRulerToWaveScroll(int value);
      void                    syncWaveToCursorTrackScroll(int value);
      void                    syncCursorTrackToWaveScroll(int value);

      void                    handleTreeSelectionChanged();
      void                    handleHierarchySelectionChanged();

      void                    handleWaveLeftMousePressed(const QPointF& scenePos,
                                                         Qt::KeyboardModifiers modifiers);
      void                    handleWaveLeftMouseDragged(const QPointF& scenePos,
                                                         Qt::KeyboardModifiers modifiers);
      void                    handleWaveLeftMouseReleased(const QPointF& scenePos,
                                                          Qt::KeyboardModifiers modifiers);

      void                    handleScopeSignalItemDoubleClicked(QTreeWidgetItem* item, int column);
      void                    handleWaveDeletePressed();

      void                    groupSelectedWaveNodes();
      void                    groupSelectedWaveNodes(const QString& groupName);
      void                    renameSelectedGroup();
      void                    ungroupSelectedWaveGroup();
      bool                    isTopLevelWaveItem(QTreeWidgetItem* item) const;

      void                    handleSignalsDroppedOnWaves(const QMimeData* mimeData,
                                                          QTreeWidgetItem* targetItem,
                                                          SignalTreeWidget::TreeDropPosition position);

      void                    doFind();      // TEMPORARY

   private:
      WaveSignal*             findSignalByPath(const QString& fullPath) const;
      std::unique_ptr<DisplayNode> makeDisplayNodeFromDropId(const QString& id) const;
      int                     topLevelIndexOfWaveItem(QTreeWidgetItem* item) const;
      bool                    insertIntoGroup(DisplayNode* group,
                                              std::unique_ptr<DisplayNode> node);
      void                    insertWaveDisplayNode(std::unique_ptr<DisplayNode> node,
                                                                    QTreeWidgetItem* targetItem,
                                                                    SignalTreeWidget::TreeDropPosition position);

      void                    buildUi();
      void                    connectUi();

      void                    rebuildTreeFromDocument();
      void                    rebuildHierarchyTree();
      void                    rebuildScopeSignalTree(DesignNode* scopeNode);
      void                    rebuildWaveNameTreeFromWaveRoots();

      std::unique_ptr<DisplayNode> cloneDisplayNode(const DisplayNode* src) const;
      void                    addDisplayNodeToWaveView(const DisplayNode* src);
      void                    addSelectedScopeSignalsToWaveView();
      void                    removeSelectedWaveNodes();
      void                    rebuildWaveDisplayRootsFromTree();
      void                    configureWaveNameTreeForReorder();

      void                    rebuildDisplayLayout();
      void                    updateScaleOnly();
      void                    updateCursorDisplays();
      void                    updateTreeValuesForActiveCursor();
      void                    updateTreeValuesForCursor(int cursorId);
      void                    updateDeltaDisplay();

      QString                 formatDisplayTime(qreal time) const;
      QString                 formatDeltaTime(qreal dt) const;
      QString                 makeDeltaText() const;

      qreal                   scaleX(qreal x) const;
      qreal                   unscaleX(qreal x) const;

      void                    applyZoom(qreal zoomStep, qreal anchorSceneX);
      const                   WaveformCursor* nearestOtherCursor(qreal time) const;

      WaveformCursor*         findCursor(int cursorId);
      const WaveformCursor*   findCursor(int cursorId) const;
      int                     findCursorNearSceneX(qreal sceneX, qreal maxDistancePx) const;
      void                    setActiveCursor(int cursorId);
      void                    setCursorTime(int cursorId, qreal time);

      bool                    snapActiveCursorToNearbyEdge(const QPointF& scenePos);
      qreal                   snapThresholdTime() const;

      void                    syncVerticalScrollRanges();
      void                    updateWaveNameBottomSpacer();
      std::unique_ptr<DisplayNode> makeSignalDisplayNode(WaveSignal* sig,
                                                         bool standaloneBitNames = false) const;
   private:
      QSplitter*              m_mainSplitter {nullptr};

      QWidget*                m_leftPane {nullptr};
      QVBoxLayout*            m_leftLayout {nullptr};
      QSplitter*              m_leftSplitter {nullptr};

      SignalTreeWidget*       m_hierarchyTree {nullptr};
      SignalTreeWidget*       m_scopeSignalTree {nullptr};

      QSplitter*              m_rightSplitter {nullptr};

      QWidget*                m_waveNamePane {nullptr};
      QVBoxLayout*            m_waveNameLayout {nullptr};
      SignalTreeWidget*       m_waveNameTree {nullptr};
      QWidget*                m_waveNameBottomSpacer {nullptr};

      QWidget*                m_rightPane {nullptr};
      QVBoxLayout*            m_rightLayout {nullptr};

      WaveDisplayWidget*      m_waveDisplay {nullptr};
      CursorTrackWidget*      m_cursorTrack {nullptr};
      TimeRulerWidget*        m_timeRuler {nullptr};

      std::unique_ptr<WaveformDocument> m_document;

      std::vector<std::unique_ptr<DisplayNode>> m_scopeDisplayRoots;
      std::vector<std::unique_ptr<DisplayNode>> m_waveDisplayRoots;

      QHash<QTreeWidgetItem*, DesignNode*>  m_itemToDesignNode;
      QHash<QTreeWidgetItem*, DisplayNode*> m_scopeItemToDisplayNode;
      QHash<QTreeWidgetItem*, DisplayNode*> m_waveItemToDisplayNode;

      QVector<WaveformCursor> m_cursors;

      int                     m_activeCursorId = 1;
      bool                    m_draggingCursor = false;

      bool                    m_syncingVerticalScroll    = false;
      bool                    m_syncingHorizontalScroll  = false;

      qreal                   m_pixelsPerUnit      = 10.0;
      qreal                   m_minPixelsPerUnit   = 0.1;
      qreal                   m_maxPixelsPerUnit   = 5000.0;

      static constexpr int    Cursor1Id               = 1;
      static constexpr int    Cursor2Id               = 2;
      static constexpr qreal  CursorTrackLaneHeight   = 20.0;
};