#pragma once

#include <QGraphicsScene>
#include <QColor>
#include <QString>
#include <QVector>

class TimeRulerScene : public QGraphicsScene
{
   Q_OBJECT


   public:
      struct RulerCursorInfo
      {
         qreal   x = 0.0;      // scene x
         QColor  color;
         QString name;
         bool    visible = true;
      };

      explicit                   TimeRulerScene(QObject* parent = nullptr);

      void                       setRulerWidth(qreal width);
      void                       setRulerHeight(qreal height);

      void                       setPixelsPerUnit(qreal pixelsPerUnit);

      void                       setMajorTickStep(qreal step);
      void                       setMinorTickStep(qreal step);

      void                       setCursors(const QVector<RulerCursorInfo>& cursors);

      void                       setSecondsPerUnit(qreal secondsPerUnit);

      void                       setDisplayUnit(const QString& unitName, qreal unitSeconds);

      void                       setAutoDisplayUnitEnabled(bool enabled);

      QString                    currentDisplayUnitName() const;
      qreal                      currentDisplayUnitSeconds() const;
      std::pair<QString, qreal>  currentDisplayUnit() const;


      qreal                      pixelsPerUnit() const { return m_pixelsPerUnit; }

      qreal                      majorTickStep() const { return m_majorTickStep; }
      qreal                      minorTickStep() const { return m_minorTickStep; }

      qreal                      rulerWidth() const { return m_rulerWidth; }
      qreal                      rulerHeight() const { return m_rulerHeight; }

      qreal                      secondsPerUnit() const { return m_secondsPerUnit; }

      QString                    displayUnitName() const { return m_displayUnitName; }
      qreal                      displayUnitSeconds() const { return m_displayUnitSeconds; }

      bool                       autoDisplayUnitEnabled() const { return m_autoDisplayUnitEnabled; }

   protected:
      void                       drawBackground(QPainter* painter, const QRectF& rect) override;
      void                       drawForeground(QPainter* painter, const QRectF& rect) override;

   private:
      qreal                      sceneXFromTime(qreal time) const;
      qreal                      timeFromSceneX(qreal x) const;
      QString                    formatTime(qreal time) const;

      qreal                      niceStep(qreal value) const;
      void                       updateTickSteps();

      qreal                      m_rulerWidth{ 4000.0 };
      qreal                      m_rulerHeight{ 24.0 };
      qreal                      m_pixelsPerUnit{ 10.0 };
      qreal                      m_majorTickStep{ 10.0 };
      qreal                      m_minorTickStep{ 2.0 };
      QString                    m_displayUnitName{ "ns" };
      qreal                      m_displayUnitSeconds{ 1e-9 };
      bool                       m_autoDisplayUnitEnabled{ true };
      QVector<RulerCursorInfo>   m_cursors;

      qreal                      m_secondsPerUnit   { 1e-9 };   // default: 1 logical unit = 1 ns
};
