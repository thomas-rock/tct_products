#include "wave_display_widget.h"

#include <QCursor>
#include <QResizeEvent>
#include <QScrollBar>
#include <QShowEvent>
#include <QVBoxLayout>
#include <QHeaderView>

#include <algorithm>
#include <cmath>

WaveDisplayWidget::WaveDisplayWidget(QWidget* parent)
   : QWidget(parent)
{
   auto* layout = new QVBoxLayout(this);
   layout->setContentsMargins(0, 0, 0, 0);
   layout->setSpacing(0);

   m_scene = new WaveScene(this);

   m_view = new WaveGraphicsView(this);
   m_view->setScene(m_scene);

   layout->addWidget(m_view);

   connect(m_view, &WaveGraphicsView::zoomChanged,
           this, &WaveDisplayWidget::handleZoomStep);

   connect(m_view, &WaveGraphicsView::leftMousePressed,
           this, &WaveDisplayWidget::leftMousePressed);
   connect(m_view, &WaveGraphicsView::leftMouseDragged,
           this, &WaveDisplayWidget::leftMouseDragged);
   connect(m_view, &WaveGraphicsView::leftMouseReleased,
           this, &WaveDisplayWidget::leftMouseReleased);

   connect(m_view, &WaveGraphicsView::middleMousePressed,
           this, &WaveDisplayWidget::handleMiddlePressed);
   connect(m_view, &WaveGraphicsView::middleMouseDragged,
           this, &WaveDisplayWidget::handleMiddleDragged);
   connect(m_view, &WaveGraphicsView::middleMouseReleased,
           this, &WaveDisplayWidget::handleMiddleReleased);
}

void WaveDisplayWidget::setWaveNameTree(QTreeWidget* tree)
{
   m_waveNameTree = tree;
}

void WaveDisplayWidget::setDisplayNodeMap(const QHash<QTreeWidgetItem*, DisplayNode*>* map)
{
   m_itemToDisplayNode = map;
}

void WaveDisplayWidget::setLogicalEndTime(qreal endTime)
{
   m_logicalEndTime = std::max<qreal>(1.0, endTime);
   rebuildSceneRect();
}

void WaveDisplayWidget::setPixelsPerUnit(qreal pixelsPerUnit)
{
   m_pixelsPerUnit = pixelsPerUnit;

   m_scene->setPixelsPerUnit(m_pixelsPerUnit);
   rebuildSceneRect();

   if (!m_lastCursors.isEmpty())
      updateCursorLines(m_lastCursors);

   m_view->viewport()->update();
}

void WaveDisplayWidget::rebuildSceneRect()
{
   if (!m_waveNameTree)
      return;

   const int viewportHeight = m_waveNameTree->viewport()->height();
   const int contentHeight =
      m_waveNameTree->verticalScrollBar()->maximum()
      + m_waveNameTree->verticalScrollBar()->pageStep();

   const qreal headerOffset = m_waveNameTree->header()->height();
   const qreal sceneHeight =
      headerOffset + std::max<qreal>(viewportHeight, contentHeight);

   m_currentSceneWaveWidth = scaleX(std::max<qreal>(100.0, m_logicalEndTime));

   m_scene->setSceneRect(0.0, 0.0, m_currentSceneWaveWidth, sceneHeight);
}

void WaveDisplayWidget::rebuildRows()
{
   if (!m_waveNameTree || !m_itemToDisplayNode)
      return;

   rebuildSceneRect();

   m_scene->clearWaveRows();

   int rowIndex = 0;
   for (int i = 0; i < m_waveNameTree->topLevelItemCount(); ++i)
      addTreeRowRecursive(m_waveNameTree->topLevelItem(i), rowIndex);

   setSelectedRowsFromTree();

   if (!m_lastCursors.isEmpty())
      updateCursorLines(m_lastCursors);

   m_view->viewport()->update();
}

void WaveDisplayWidget::addTreeRowRecursive(QTreeWidgetItem* item, int& rowIndex)
{
   if (!item)
      return;

   if (isItemVisibleByExpansion(item))
   {
      const QRect vr = m_waveNameTree->visualItemRect(item);
      if (vr.isValid() && !vr.isEmpty())
      {
         const qreal headerOffset = m_waveNameTree->header()->height();
         const qreal y = headerOffset + vr.top() + TrackTopPadding;
         const qreal h = std::max<qreal>(
            6.0,
            vr.height() - TrackTopPadding - TrackBottomPadding);

         DisplayNode* displayNode = m_itemToDisplayNode->value(item, nullptr);

         if (displayNode &&
             displayNode->signal &&
             (displayNode->kind == DisplayNodeKind::Signal ||
              displayNode->kind == DisplayNodeKind::Field ||
              displayNode->kind == DisplayNodeKind::Bit))
         {
            m_scene->addDisplayNodeRow(item,
                                       displayNode,
                                       rowIndex,
                                       WaveLeftMargin,
                                       y,
                                       h,
                                       m_currentSceneWaveWidth - WaveLeftMargin,
                                       m_pixelsPerUnit);
         }

         ++rowIndex;
      }
   }

   for (int i = 0; i < item->childCount(); ++i)
      addTreeRowRecursive(item->child(i), rowIndex);
}

bool WaveDisplayWidget::isItemVisibleByExpansion(QTreeWidgetItem* item) const
{
   for (QTreeWidgetItem* p = item ? item->parent() : nullptr; p; p = p->parent())
   {
      if (!p->isExpanded())
         return false;
   }

   return true;
}

void WaveDisplayWidget::setSelectedRowsFromTree()
{
   if (!m_waveNameTree)
      return;

   for (int i = 0; i < m_waveNameTree->topLevelItemCount(); ++i)
   {
      QList<QTreeWidgetItem*> stack;
      stack.append(m_waveNameTree->topLevelItem(i));

      while (!stack.isEmpty())
      {
         QTreeWidgetItem* item = stack.takeLast();
         m_scene->setRowHighlight(item, item->isSelected());

         for (int c = 0; c < item->childCount(); ++c)
            stack.append(item->child(c));
      }
   }
}

void WaveDisplayWidget::ensureCursorItems(const QVector<WaveformCursor>& cursors)
{
   const qreal cursorHeight =
      std::max<qreal>(m_scene->sceneRect().height(), m_view->viewport()->height());

   for (const auto& cursor : cursors)
   {
      if (m_waveCursorItems.contains(cursor.id))
         continue;

      auto* item = new CursorLineItem(cursorHeight, cursor.color);
      item->setZValue(9000.0);
      m_scene->addItem(item);
      m_waveCursorItems.insert(cursor.id, item);
   }
}

void WaveDisplayWidget::updateCursorLines(const QVector<WaveformCursor>& cursors)
{
   m_lastCursors = cursors;

   ensureCursorItems(cursors);

   const qreal cursorHeight =
      std::max<qreal>(m_scene->sceneRect().height(), m_view->viewport()->height());

   for (const auto& cursor : cursors)
   {
      CursorLineItem* item = m_waveCursorItems.value(cursor.id, nullptr);
      if (!item)
         continue;

      item->setVisible(cursor.visible);
      item->setColor(cursor.color);
      item->setHeight(cursorHeight);
      item->setPos(scaleX(cursor.time), 0.0);
   }
}

QTreeWidgetItem* WaveDisplayWidget::itemAtScenePos(const QPointF& scenePos) const
{
   return m_scene->itemAtScenePos(scenePos);
}

QScrollBar* WaveDisplayWidget::horizontalScrollBar()
{
   return m_view->horizontalScrollBar();
}

QScrollBar* WaveDisplayWidget::verticalScrollBar()
{
   return m_view->verticalScrollBar();
}

QWidget* WaveDisplayWidget::viewport()
{
   return m_view->viewport();
}

void WaveDisplayWidget::resizeEvent(QResizeEvent* event)
{
   QWidget::resizeEvent(event);

   rebuildSceneRect();

   if (!m_lastCursors.isEmpty())
      updateCursorLines(m_lastCursors);
}

void WaveDisplayWidget::showEvent(QShowEvent* event)
{
   QWidget::showEvent(event);

   rebuildSceneRect();

   if (!m_lastCursors.isEmpty())
      updateCursorLines(m_lastCursors);
}

void WaveDisplayWidget::handleZoomStep(qreal step)
{
   const QPoint viewportPos = m_view->viewport()->mapFromGlobal(QCursor::pos());
   const QPointF scenePos = m_view->mapToScene(viewportPos);

   emit zoomStepRequested(step, scenePos.x());
}

void WaveDisplayWidget::handleMiddlePressed(const QPointF& scenePos,
                                            Qt::KeyboardModifiers modifiers)
{
   Q_UNUSED(modifiers)
   beginAreaZoom(scenePos);
}

void WaveDisplayWidget::handleMiddleDragged(const QPointF& scenePos,
                                            Qt::KeyboardModifiers modifiers)
{
   Q_UNUSED(modifiers)
   updateAreaZoom(scenePos);
}

void WaveDisplayWidget::handleMiddleReleased(const QPointF& scenePos,
                                             Qt::KeyboardModifiers modifiers)
{
   Q_UNUSED(modifiers)
   finishAreaZoom(scenePos);
}

void WaveDisplayWidget::beginAreaZoom(const QPointF& scenePos)
{
   cancelAreaZoom();

   m_areaZooming = true;
   m_areaZoomStartScene = scenePos;

   const QRectF r(scenePos.x(),
                  m_scene->sceneRect().top(),
                  0.0,
                  m_scene->sceneRect().height());

   m_areaZoomRect = m_scene->addRect(
      r,
      QPen(QColor(80, 160, 255), 1.0, Qt::DashLine),
      QBrush(QColor(80, 160, 255, 40)));

   m_areaZoomRect->setZValue(10000.0);
}

void WaveDisplayWidget::updateAreaZoom(const QPointF& scenePos)
{
   if (!m_areaZooming || !m_areaZoomRect)
      return;

   const qreal x0 = m_areaZoomStartScene.x();
   const qreal x1 = scenePos.x();

   const qreal left = std::min(x0, x1);
   const qreal right = std::max(x0, x1);

   m_areaZoomRect->setRect(QRectF(left,
                                  m_scene->sceneRect().top(),
                                  right - left,
                                  m_scene->sceneRect().height()));
}

void WaveDisplayWidget::finishAreaZoom(const QPointF& scenePos)
{
   if (!m_areaZooming)
      return;

   const qreal x0 = m_areaZoomStartScene.x();
   const qreal x1 = scenePos.x();

   cancelAreaZoom();

   constexpr qreal MinDragPixels = 8.0;
   if (std::abs(x1 - x0) < MinDragPixels)
      return;

   qreal t0 = unscaleX(x0);
   qreal t1 = unscaleX(x1);

   if (t1 < t0)
      std::swap(t0, t1);

   emit areaZoomRequested(t0, t1);
}

void WaveDisplayWidget::cancelAreaZoom()
{
   m_areaZooming = false;

   if (m_areaZoomRect)
   {
      m_scene->removeItem(m_areaZoomRect);
      delete m_areaZoomRect;
      m_areaZoomRect = nullptr;
   }
}
