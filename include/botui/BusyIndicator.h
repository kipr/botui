// Modification of the BusyIndicator code snippet found on Qt's website
// (http://qt-project.org/wiki/Busy_Indicator_for_QML)

#ifndef _BUSYINDICATOR_H_
#define _BUSYINDICATOR_H_

#include <QStyleOptionGraphicsItem>
#include <QtQuick/QQuickItem>

class BusyIndicator : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(qreal innerRadius READ innerRadius WRITE setInnerRadius NOTIFY
                   innerRadiusChanged)
    Q_PROPERTY(qreal outerRadius READ outerRadius WRITE setOuterRadius NOTIFY
                   outerRadiusChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE
                   setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE
                   setForegroundColor NOTIFY foregroundColorChanged)
    Q_PROPERTY(qreal actualInnerRadius READ actualInnerRadius NOTIFY
                   actualInnerRadiusChanged)
    Q_PROPERTY(qreal actualOuterRadius READ actualOuterRadius NOTIFY
                   actualOuterRadiusChanged)

  public:
    explicit BusyIndicator(QQuickItem *parent = 0);

    void setInnerRadius(const qreal &innerRadius);
    qreal innerRadius() const;

    void setOuterRadius(const qreal &outerRadius);
    qreal outerRadius() const;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    void setForegroundColor(const QColor &color);
    QColor foregroundColor() const;

    qreal actualInnerRadius() const;
    qreal actualOuterRadius() const;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget = 0);

  signals:
    void innerRadiusChanged();
    void outerRadiusChanged();
    void backgroundColorChanged();
    void foregroundColorChanged();
    void actualInnerRadiusChanged();
    void actualOuterRadiusChanged();

  protected slots:
    virtual void updateSpinner();

  private:
    // User settable properties
    qreal m_innerRadius; // In range (0, m_outerRadius]
    qreal m_outerRadius; // (m_innerRadius, 1]
    QColor m_backgroundColor;
    QColor m_foregroundColor;

    // The calculated size, inner and outer radii
    qreal m_size;
    qreal m_actualInnerRadius;
    qreal m_actualOuterRadius;

    QString m_cacheKey;
};

#endif
