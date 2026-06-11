#pragma once

#include <QGraphicsScene>
#include <QHash>
#include <QTreeWidgetItem>
#include <QString>

#include "waveform_model.h"
#include "wave_query_engine.h"

class WaveTrackItem;

class WaveScene : public QGraphicsScene
{
   Q_OBJECT

   public:
      explicit             WaveScene(QObject* parent = nullptr);

      void                 clearWaveRows();

      WaveTrackItem*       addDisplayNodeRow(QTreeWidgetItem* item,
                                             DisplayNode* displayNode,
                                             int rowIndex,
                                             qreal x,
                                             qreal y,
                                             qreal height,
                                             qreal width,
                                             qreal pixelsPerUnit);

      void                 setRowHighlight(QTreeWidgetItem* item, bool highlighted);

      WaveTrackItem*       trackForItem(QTreeWidgetItem* item) const;
      QString              valueAt(QTreeWidgetItem* item, qreal x) const;
      QTreeWidgetItem*     itemAtScenePos(const QPointF& scenePos) const;
      void                 setPixelsPerUnit(qreal pixelsPerUnit);

      void                 setQueryMatches(const QVector<WaveQueryEngine::WaveQueryMatch>& matches);
      void                 clearQueryMatches();

   protected:
      void drawBackground(QPainter* painter, const QRectF& rect) override;

   private:
      QHash<QTreeWidgetItem*, WaveTrackItem*>   m_rows;
      QVector<WaveQueryEngine::WaveQueryMatch>  m_queryMatches;
      qreal                                     m_pixelsPerUnit;
};