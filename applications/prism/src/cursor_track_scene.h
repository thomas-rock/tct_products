#pragma once

#include <QGraphicsScene>
#include <QColor>
#include <QString>
#include <QVector>

class CursorTrackScene : public QGraphicsScene
{
   Q_OBJECT


   public:
      struct   CursorTrackInfo
      {
         int      id = 0;
         qreal    x  = 0.0;          // scene x
         QColor   color;
         QString  label;
         QString  deltaLabel;
         bool     visible = true;
         bool     locked  = false;
         bool     active  = false;
      };

   explicit       CursorTrackScene(QObject* parent = nullptr);

   void           setTrackWidth(qreal width);
   void           setLaneHeight(qreal height);
   void           setCursors(const QVector<CursorTrackInfo>& cursors);

   qreal          trackWidth() const { return m_trackWidth; }
   qreal          laneHeight() const { return m_laneHeight; }
   qreal          totalHeight() const;

   protected:
      void        drawBackground(QPainter* painter, const QRectF& rect) override;
      void        drawForeground(QPainter* painter, const QRectF& rect) override;

   private:
      qreal       m_trackWidth = 4000.0;
      qreal       m_laneHeight = 20.0;
      QVector<CursorTrackInfo> m_cursors;
};
