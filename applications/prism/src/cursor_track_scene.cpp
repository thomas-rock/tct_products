#include "cursor_track_scene.h"

#include <QApplication>
#include <QPalette>
#include <QPainter>
#include <QFontMetricsF>
#include <algorithm>

CursorTrackScene::CursorTrackScene(QObject* parent)
    : QGraphicsScene(parent)
{
    setSceneRect(0.0, 0.0, m_trackWidth, totalHeight());
}

void CursorTrackScene::setTrackWidth(qreal width)
{
    m_trackWidth = width;
    setSceneRect(sceneRect().x(), 0.0, m_trackWidth, totalHeight());
    update();
}

void CursorTrackScene::setLaneHeight(qreal height)
{
    m_laneHeight = height;
    setSceneRect(sceneRect().x(), 0.0, m_trackWidth, totalHeight());
    update();
}

void CursorTrackScene::setCursors(const QVector<CursorTrackInfo>& cursors)
{
    m_cursors = cursors;
    setSceneRect(sceneRect().x(), 0.0, m_trackWidth, totalHeight());
    update();
}

qreal CursorTrackScene::totalHeight() const
{
    int visibleCount = 0;
    for (const auto& c : m_cursors)
    {
        if (c.visible)
            ++visibleCount;
    }

    if (visibleCount <= 0)
        visibleCount = 1;

    return visibleCount * m_laneHeight;
}

void CursorTrackScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    Q_UNUSED(rect)
    painter->fillRect(sceneRect(), qApp->palette().base());
}

void CursorTrackScene::drawForeground(QPainter* painter, const QRectF& rect)
{
    Q_UNUSED(rect)

    const QColor textColor = qApp->palette().text().color();
    const QColor lineColor = qApp->palette().mid().color();
    const QColor activeFill(80, 100, 140, 60);

    painter->setRenderHint(QPainter::Antialiasing, false);

    QFontMetricsF fm(painter->font());

    int laneIndex = 0;
    for (const auto& c : m_cursors)
    {
        if (!c.visible)
            continue;

        const qreal yTop = laneIndex * m_laneHeight;
        const qreal yBottom = yTop + m_laneHeight;
        const qreal yMid = 0.5 * (yTop + yBottom);

        // Active lane highlight
        if (c.active)
            painter->fillRect(QRectF(sceneRect().left(), yTop, sceneRect().width(), m_laneHeight),
                              activeFill);

        // Lane separator
        painter->setPen(lineColor);
        painter->drawLine(QPointF(sceneRect().left(), yBottom),
                          QPointF(sceneRect().right(), yBottom));

        // Cursor line through the full cursor-track area
        QPen cursorPen(c.color);
        cursorPen.setWidthF(c.active ? 2.0 : 1.0);
        painter->setPen(cursorPen);
        painter->drawLine(QPointF(c.x, sceneRect().top() + 1.0),
                          QPointF(c.x, sceneRect().bottom() - 1.0));

        // Cursor label placement based on viewport/visible center
        const qreal visibleCenterX = rect.center().x();
        QString label = c.locked ? QString("%1 [L]").arg(c.label) : c.label;
        if (!c.deltaLabel.isEmpty())
           label += QString("    %1").arg(c.deltaLabel);

        painter->setPen(textColor);
        const QRectF textRect = fm.boundingRect(label);

        qreal textX = 0.0;
        if (c.x < visibleCenterX)
           textX = c.x + 4.0;
        else
           textX = c.x - textRect.width() - 4.0;

        // Clamp text into visible scene rect a little
        textX = std::max(sceneRect().left() + 2.0,
                         std::min(textX, sceneRect().right() - textRect.width() - 2.0));

        const qreal textY = yMid + (fm.ascent() - fm.descent()) * 0.5;
        painter->drawText(QPointF(textX, textY), label);

        ++laneIndex;
    }
}
