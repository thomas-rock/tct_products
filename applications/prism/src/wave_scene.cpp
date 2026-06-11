#include "wave_scene.h"
#include "wave_track_item.h"
#include "bus_renderer.h"
#include "bit_renderer.h"
#include "waveform_model.h"

#include <memory>

WaveScene::WaveScene(QObject* parent)
   : QGraphicsScene(parent)
{
}

void WaveScene::clearWaveRows()
{
   for (auto it = m_rows.begin(); it != m_rows.end(); ++it)
   {
      if (it.value())
      {
         removeItem(it.value());
         delete it.value();
      }
   }

   m_rows.clear();
}

WaveTrackItem* WaveScene::addDisplayNodeRow(QTreeWidgetItem* item,
                                            DisplayNode* displayNode,
                                            int rowIndex,
                                            qreal x,
                                            qreal y,
                                            qreal height,
                                            qreal width,
                                            qreal pixelsPerUnit)
{
   Q_UNUSED(rowIndex)

   if (!displayNode || !displayNode->signal)
      return nullptr;

   std::unique_ptr<IWaveRenderer> renderer;

   if (displayNode->kind == DisplayNodeKind::Bit)
   {
      auto bit = std::make_unique<BitRenderer>();
      bit->setStyle(displayNode->signal->style);
      bit->setPixelsPerUnit(pixelsPerUnit);

      if (displayNode->signal->kind == WaveSignalKind::Bit)
         bit->setSegments(displayNode->signal->segments);
      else
         bit->setBusBitSegments(displayNode->signal->segments, displayNode->msb);

      renderer = std::move(bit);
   }
   else if (displayNode->kind == DisplayNodeKind::Field)
   {
      const int width = displayNode->msb - displayNode->lsb + 1;

      if (width <= 1)
      {
         auto bit = std::make_unique<BitRenderer>();
         bit->setStyle(displayNode->signal->style);
         bit->setPixelsPerUnit(pixelsPerUnit);
         bit->setBusBitSegments(displayNode->signal->segments, displayNode->msb);
         renderer = std::move(bit);
      }
      else
      {
         auto bus = std::make_unique<BusRenderer>();
         bus->setStyle(displayNode->signal->style);
         bus->setPixelsPerUnit(pixelsPerUnit);
         bus->setFieldSegments(displayNode->signal->segments,
                               displayNode->msb,
                               displayNode->lsb);
         renderer = std::move(bus);
      }
   }
   else if (displayNode->kind == DisplayNodeKind::Signal)
   {
      if (displayNode->signal->kind == WaveSignalKind::Bit)
      {
         auto bit = std::make_unique<BitRenderer>();
         bit->setStyle(displayNode->signal->style);
         bit->setPixelsPerUnit(pixelsPerUnit);
         bit->setSegments(displayNode->signal->segments);
         renderer = std::move(bit);
      }
      else
      {
         auto bus = std::make_unique<BusRenderer>();
         bus->setStyle(displayNode->signal->style);
         bus->setPixelsPerUnit(pixelsPerUnit);
         bus->setSegments(displayNode->signal->segments);
         renderer = std::move(bus);
      }
   }

   if (!renderer)
      return nullptr;

   QRectF rowRect(x, y, width, height);

   auto* track = new WaveTrackItem(rowRect, std::move(renderer));
   addItem(track);

   m_rows.insert(item, track);
   return track;
}

void WaveScene::setRowHighlight(QTreeWidgetItem* item, bool highlighted)
{
   auto it = m_rows.find(item);
   if (it == m_rows.end())
      return;

   it.value()->setSelectedRow(highlighted);
}

WaveTrackItem* WaveScene::trackForItem(QTreeWidgetItem* item) const
{
   auto it = m_rows.find(item);
   if (it == m_rows.end())
      return nullptr;

   return it.value();
}

QString WaveScene::valueAt(QTreeWidgetItem* item, qreal x) const
{
   auto it = m_rows.find(item);
   if (it == m_rows.end())
      return {};

   WaveTrackItem* track = it.value();
   if (!track || !track->renderer())
      return {};

   return track->renderer()->valueAt(x);
}

QTreeWidgetItem* WaveScene::itemAtScenePos(const QPointF& scenePos) const
{
   for (auto it = m_rows.begin(); it != m_rows.end(); ++it)
   {
      WaveTrackItem* track = it.value();
      if (!track)
         continue;

      const QRectF sceneBounds = track->sceneBoundingRect();
      if (sceneBounds.contains(scenePos))
         return it.key();
   }

   return nullptr;
}

void WaveScene::setPixelsPerUnit(qreal pixelsPerUnit)
{
   for (auto it = m_rows.begin(); it != m_rows.end(); ++it)
   {
      if (it.value())
         it.value()->setPixelsPerUnit(pixelsPerUnit);
   }

   m_pixelsPerUnit = pixelsPerUnit;

   update();
}

void WaveScene::setQueryMatches(const QVector<WaveQueryEngine::WaveQueryMatch>& matches)
{
   m_queryMatches = matches;
   update();
}

void WaveScene::clearQueryMatches()
{
   m_queryMatches.clear();
   update();
}

void WaveScene::drawBackground(QPainter* painter, const QRectF& rect)
{
   for (const auto& match : m_queryMatches)
   {
      const qreal x0 = match.startTime * m_pixelsPerUnit;
      const qreal x1 = match.endTime * m_pixelsPerUnit;

      QRectF r(x0, rect.top(), x1 - x0, rect.height());
      painter->fillRect(r, QColor(255, 230, 120, 60));
   }
}