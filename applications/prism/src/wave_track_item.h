#pragma once

#include "iwave_renderer.h"

#include <QGraphicsObject>
#include <memory>

class WaveTrackItem : public QGraphicsObject
{
   Q_OBJECT

   public:
      explicit             WaveTrackItem(const QRectF& rowRect,
                                         std::unique_ptr<IWaveRenderer> renderer,
                                         QGraphicsItem* parent = nullptr);

      QRectF               boundingRect() const override;
      void                 paint(QPainter* painter,
                                 const QStyleOptionGraphicsItem* option,
                                 QWidget* widget = nullptr) override;

      void                 setPixelsPerUnit(qreal pixelsPerUnit);
      void                 setRowRect(const QRectF& rect);
      QRectF               rowRect() const { return m_rowRect; }

      void                 setSelectedRow(bool selected);
      bool                 isSelectedRow() const { return m_selected; }

      IWaveRenderer*       renderer() { return m_renderer.get(); }
      const IWaveRenderer* renderer() const { return m_renderer.get(); }

      template<typename T>
      T* rendererAs()
      {
         return dynamic_cast<T*>(m_renderer.get());
      }

      template<typename T>
      const T* rendererAs() const
      {
         return dynamic_cast<const T*>(m_renderer.get());
      }

   private:
      QRectF                           m_rowRect;   // local coordinates: usually (0,0,w,h)
      std::unique_ptr<IWaveRenderer>   m_renderer;
      bool                             m_selected {false};
};