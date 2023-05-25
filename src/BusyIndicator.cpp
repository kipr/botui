#include "BusyIndicator.h"

#include <QConicalGradient>
#include <QGraphicsItem>
#include <QPainter>
#include <QPainterPath>
#include <QPixmapCache>
#include <QStyleOptionGraphicsItem>

BusyIndicator::BusyIndicator(QQuickItem *parent)
    : QQuickItem(parent),
      m_innerRadius(0.8),
      m_outerRadius(1.0),
      m_backgroundColor(177, 210, 143, 70),
      m_foregroundColor(119, 183, 83, 255),
      m_actualInnerRadius(90.0),
      m_actualOuterRadius(100.0),
      m_cacheKey() {
    setFlag(QQuickItem::ItemHasContents, true);
    setWidth(100.0);
    setHeight(100.0);

    updateSpinner();

    connect(this, SIGNAL(widthChanged()), SLOT(updateSpinner()));
    connect(this, SIGNAL(heightChanged()), SLOT(updateSpinner()));
}

void BusyIndicator::setInnerRadius(const qreal &innerRadius) {
    if (qFuzzyCompare(m_innerRadius, innerRadius)) return;
    m_innerRadius = innerRadius;
    updateSpinner();
    emit innerRadiusChanged();
}

qreal BusyIndicator::innerRadius() const { return m_innerRadius; }

void BusyIndicator::setOuterRadius(const qreal &outerRadius) {
    if (qFuzzyCompare(m_outerRadius, outerRadius)) return;
    m_outerRadius = outerRadius;
    updateSpinner();
    emit outerRadiusChanged();
}

qreal BusyIndicator::outerRadius() const { return m_outerRadius; }

void BusyIndicator::setBackgroundColor(const QColor &color) {
    if (m_backgroundColor == color) return;
    m_backgroundColor = color;
    updateSpinner();
    emit backgroundColorChanged();
}

QColor BusyIndicator::backgroundColor() const { return m_backgroundColor; }

void BusyIndicator::setForegroundColor(const QColor &color) {
    if (m_foregroundColor == color) return;
    m_foregroundColor = color;
    updateSpinner();
    emit foregroundColorChanged();
}

QColor BusyIndicator::foregroundColor() const { return m_foregroundColor; }

qreal BusyIndicator::actualInnerRadius() const { return m_actualInnerRadius; }

qreal BusyIndicator::actualOuterRadius() const { return m_actualOuterRadius; }

void BusyIndicator::updateSpinner() {
    // Calculate new inner and outer radii
    m_size = qMin(width(), height());
    qreal nCoef = 0.5 * m_size;
    m_actualInnerRadius = nCoef * m_innerRadius;
    m_actualOuterRadius = nCoef * m_outerRadius;

    // Calculate a new key
    m_cacheKey = m_backgroundColor.name();
    m_cacheKey += "-";
    m_cacheKey += m_foregroundColor.name();
    m_cacheKey += "-";
    m_cacheKey += QString::number(m_actualOuterRadius);
    m_cacheKey += "-";
    m_cacheKey += QString::number(m_actualInnerRadius);

    emit actualInnerRadiusChanged();
    emit actualOuterRadiusChanged();
}

void BusyIndicator::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget) {
    Q_UNUSED(option);

    // Set up a convenient path
    QPainterPath path;
    path.setFillRule(Qt::OddEvenFill);
    path.addEllipse(QPointF(m_actualOuterRadius, m_actualOuterRadius),
                    m_actualOuterRadius,
                    m_actualOuterRadius);
    path.addEllipse(QPointF(m_actualOuterRadius, m_actualOuterRadius),
                    m_actualInnerRadius,
                    m_actualInnerRadius);

    // Draw the ring background
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);

    // Draw the ring foreground
    QConicalGradient gradient(QPointF(m_actualOuterRadius, m_actualOuterRadius),
                              0.0);
    gradient.setColorAt(0.0, Qt::transparent);
    gradient.setColorAt(0.05, m_foregroundColor);
    gradient.setColorAt(0.7, Qt::transparent);
    painter->setBrush(gradient);
    painter->drawPath(path);
}
