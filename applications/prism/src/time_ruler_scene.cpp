#include "time_ruler_scene.h"

#include <QApplication>
#include <QPalette>
#include <QPainter>
#include <QFontMetricsF>
#include <QtMath>
#include <algorithm>
#include <cmath>

TimeRulerScene::TimeRulerScene(QObject* parent)
   : QGraphicsScene(parent)
{
   setSceneRect(0.0, 0.0, m_rulerWidth, m_rulerHeight);
   updateTickSteps();
}

void TimeRulerScene::setRulerWidth(qreal width)
{
   m_rulerWidth = width;
   setSceneRect(sceneRect().x(), 0.0, m_rulerWidth, m_rulerHeight);
   update();
}

void TimeRulerScene::setRulerHeight(qreal height)
{
   m_rulerHeight = height;
   setSceneRect(sceneRect().x(), 0.0, sceneRect().width(), m_rulerHeight);
   update();
}

void TimeRulerScene::setPixelsPerUnit(qreal pixelsPerUnit)
{
   m_pixelsPerUnit = std::max<qreal>(0.0001, pixelsPerUnit);
   updateTickSteps();
   update();
}

void TimeRulerScene::setMajorTickStep(qreal step)
{
   m_majorTickStep = std::max<qreal>(0.0001, step);
   update();
}

void TimeRulerScene::setMinorTickStep(qreal step)
{
   m_minorTickStep = std::max<qreal>(0.0001, step);
   update();
}

void TimeRulerScene::setCursors(const QVector<RulerCursorInfo>& cursors)
{
   m_cursors = cursors;
   update();
}

void TimeRulerScene::setSecondsPerUnit(qreal secondsPerUnit)
{
   m_secondsPerUnit = std::max<qreal>(secondsPerUnit, 1e-30);
   update();
}

void TimeRulerScene::setDisplayUnit(const QString& unitName, qreal unitSeconds)
{
   m_displayUnitName = unitName;
   m_displayUnitSeconds = std::max<qreal>(unitSeconds, 1e-30);
   update();
}

void TimeRulerScene::setAutoDisplayUnitEnabled(bool enabled)
{
   m_autoDisplayUnitEnabled = enabled;
   update();
}

QString TimeRulerScene::currentDisplayUnitName() const
{
   return currentDisplayUnit().first;
}

qreal TimeRulerScene::currentDisplayUnitSeconds() const
{
   return currentDisplayUnit().second;
}

std::pair<QString, qreal> TimeRulerScene::currentDisplayUnit() const
{
   if (!m_autoDisplayUnitEnabled)
      return {m_displayUnitName, m_displayUnitSeconds};

   const qreal majorStepSeconds = m_majorTickStep * m_secondsPerUnit;

   if (majorStepSeconds < 1e-9)
      return {"ps", 1e-12};
   else if (majorStepSeconds < 1e-6)
      return {"ns", 1e-9};
   else if (majorStepSeconds < 1e-3)
      return {"us", 1e-6};
   else if (majorStepSeconds < 1.0)
      return {"ms", 1e-3};
   else
      return {"s", 1.0};
}

qreal TimeRulerScene::sceneXFromTime(qreal time) const
{
   return time * m_pixelsPerUnit;
}

qreal TimeRulerScene::timeFromSceneX(qreal x) const
{
   if (m_pixelsPerUnit == 0.0)
      return 0.0;

   return x / m_pixelsPerUnit;
}

QString TimeRulerScene::formatTime(qreal time) const
{
   const auto [unitName, unitSeconds] = currentDisplayUnit();

   const qreal seconds = time * m_secondsPerUnit;
   const qreal displayValue = seconds / unitSeconds;

   const qreal majorStepDisplay =
      (m_majorTickStep * m_secondsPerUnit) / unitSeconds;

   int decimals = 0;

   if (majorStepDisplay >= 100.0)
      decimals = 0;
   else if (majorStepDisplay >= 10.0)
      decimals = 1;
   else if (majorStepDisplay >= 1.0)
      decimals = 2;
   else
      decimals = std::clamp(
         int(std::ceil(-std::log10(std::max(majorStepDisplay, 1e-12)))) + 1,
         2, 6);

   return QString("%1 %2")
      .arg(displayValue, 0, 'f', decimals)
      .arg(unitName);
}

qreal TimeRulerScene::niceStep(qreal value) const
{
   if (value <= 0.0)
      return 1.0;

   const qreal exponent = std::floor(std::log10(value));
   const qreal base = std::pow(10.0, exponent);
   const qreal fraction = value / base;

   qreal niceFraction = 1.0;

   if (fraction <= 1.0)
      niceFraction = 1.0;
   else if (fraction <= 2.0)
      niceFraction = 2.0;
   else if (fraction <= 5.0)
      niceFraction = 5.0;
   else
      niceFraction = 10.0;

   return niceFraction * base;
}

void TimeRulerScene::updateTickSteps()
{
   constexpr qreal targetMajorSpacingPx = 90.0;

   const qreal desiredMajorStep = targetMajorSpacingPx / m_pixelsPerUnit;

   m_majorTickStep = niceStep(desiredMajorStep);
   m_minorTickStep = m_majorTickStep / 5.0;

   if (m_minorTickStep <= 0.0)
      m_minorTickStep = m_majorTickStep;
}

void TimeRulerScene::drawBackground(QPainter* painter, const QRectF& rect)
{
   Q_UNUSED(rect)
   painter->fillRect(sceneRect(), palette().base());
}

void TimeRulerScene::drawForeground(QPainter* painter, const QRectF& rect)
{
   painter->setRenderHint(QPainter::Antialiasing, false);

   const QRectF sr = sceneRect();
   const qreal bottom = sr.bottom();
   const qreal top = sr.top();

   const QColor textColor = qApp->palette().text().color();
   const QColor tickColor = qApp->palette().mid().color();

   // Bottom baseline
   painter->setPen(QPen(tickColor));
   painter->drawLine(QPointF(sr.left(), bottom), QPointF(sr.right(), bottom));

   QFontMetricsF fm(painter->font());

   // Visible scene range -> visible logical time range
   const qreal leftSceneX  = rect.left();
   const qreal rightSceneX = rect.right();

   const qreal leftTime  = timeFromSceneX(leftSceneX);
   const qreal rightTime = timeFromSceneX(rightSceneX);

   if (m_minorTickStep <= 0.0 || m_majorTickStep <= 0.0)
      return;

   const qreal firstMinorTime =
      std::floor(leftTime / m_minorTickStep) * m_minorTickStep;
   const qreal lastMinorTime =
      std::ceil(rightTime / m_minorTickStep) * m_minorTickStep;

   // Tick lines + major labels
   for (qreal t = firstMinorTime; t <= lastMinorTime + 0.5 * m_minorTickStep; t += m_minorTickStep)
   {
      const qreal x = sceneXFromTime(t);

      const qreal majorRemainder = std::fmod(std::abs(t), m_majorTickStep);
      const bool isMajor =
         (majorRemainder < 0.0001) ||
         (std::abs(majorRemainder - m_majorTickStep) < 0.0001);

      const qreal tickTop = isMajor ? top + 10.0 : top + 18.0;

      painter->setPen(QPen(tickColor));
      painter->drawLine(QPointF(x, tickTop), QPointF(x, bottom));

      if (isMajor)
      {
         const QString label = formatTime(t);
         painter->setPen(textColor);
         painter->drawText(QPointF(x + 2.0, top + fm.ascent() + 1.0), label);
      }
   }
}