#include "BatteryWidget.h"
#include "BatteryLevelProvider.h"

#include "Device.h"
#include "WombatBatteryProvider.h"
#include "kipr/wombat.h"
#include <kipr/battery/battery.h>

#include <QDebug>
#include <QPainter>
#include <QTimer>

#define CAPACITY_TEXT_SIZE 35
#define CHEMISTRY_TEXT_SIZE 35

#define BATTERY_TYPE_KEY "battery_type"

BatteryWidget::BatteryWidget(QWidget *parent)
    : QWidget(parent), m_batteryLevelProvider(0) {
    startTimer();
}

BatteryWidget::BatteryWidget(BatteryLevelProvider *batteryLevelProvider,
                             QWidget *parent)
    : QWidget(parent), m_batteryLevelProvider(batteryLevelProvider) {
    setMinimumSize(25 + CAPACITY_TEXT_SIZE + CHEMISTRY_TEXT_SIZE, 14);
    setMaximumSize(25 + CAPACITY_TEXT_SIZE + CHEMISTRY_TEXT_SIZE, 14);

    startTimer();
}

void BatteryWidget::setBatteryLevelProvider(
    BatteryLevelProvider *batteryLevelProvider) {
    m_batteryLevelProvider = batteryLevelProvider;
}

BatteryLevelProvider *BatteryWidget::batteryLevelProvider() const {
    return m_batteryLevelProvider;
}

void BatteryWidget::paintEvent(QPaintEvent *event) {
    const float batteryLevel =
        m_batteryLevelProvider ? m_batteryLevelProvider->batteryLevel() : 0.0;

    float percentage = m_batteryLevelProvider ? batteryLevel : 0.0;

    percentage = percentage < 0 ? 0 : percentage;
    percentage = percentage > 1.0 ? 1.0 : percentage;

    const int w = width();
    const int h = height();

    const static int connectorSize = 3; // the little +V battery nub
    const static int textSize =
        CAPACITY_TEXT_SIZE; // width of the battery % text

    QPainter painter(this);
    // painter.setRenderHint(QPainter::Antialiasing);

    QRectF battTypeTextRect(0, 0, CHEMISTRY_TEXT_SIZE, h);
    QTextOption battTypeTextOpt(Qt::AlignAbsolute | Qt::AlignHCenter |
                                Qt::AlignVCenter);

    QString battTypeString = QString("    ");

    int battTypeInt =
        ((Wombat::BatteryLevelProvider *)m_batteryLevelProvider)->batteryType();

    switch (battTypeInt) {
    case 0:
        battTypeString = QString("LiFe");
        break;
    case 1:
        battTypeString = QString("LiPo");
        break;
    case 2:
        battTypeString = QString("NiMH");
        break;
    default:
        break;
    }

    painter.drawText(
        battTypeTextRect,
        battTypeString,
        battTypeTextOpt); // TODO: get battery type string elsewhere

    const int adjustedWidth =
        w - connectorSize - textSize - CHEMISTRY_TEXT_SIZE;
    const int battOffset = 0 + CHEMISTRY_TEXT_SIZE;
    QPointF polygon[8] = {
        QPointF(0 + battOffset, 0),
        QPointF(adjustedWidth + battOffset, 0),
        QPointF(adjustedWidth + battOffset, h / 4.0),
        QPointF(adjustedWidth + battOffset + connectorSize, h / 4.0),
        QPointF(adjustedWidth + battOffset + connectorSize, 3.0 * h / 4.0),
        QPointF(adjustedWidth + battOffset, 3.0 * h / 4.0),
        QPointF(adjustedWidth + battOffset, h - 1),
        QPointF(0 + battOffset, h - 1)};
    painter.setPen(QColor(30, 30, 30));
    painter.setBrush(Qt::darkGray);
    painter.drawPolygon(polygon, 8);

    const int offset = 2;
    painter.setPen(Qt::NoPen);

    const bool charging = m_batteryLevelProvider->isCharging();

    if (m_batteryLevelProvider && charging)
        painter.setBrush(QColor(0x7D, 0xF9, 0xFF)); // Electric Blue
    else if (percentage < 0.33)
        painter.setBrush(Qt::red);
    else if (percentage < 0.66)
        painter.setBrush(Qt::yellow);
    else
        painter.setBrush(Qt::green);

    const float barWidth = adjustedWidth * percentage;
    QRect rect(offset + CHEMISTRY_TEXT_SIZE,
               offset,
               (barWidth > offset ? barWidth : offset) - offset - 1,
               h - offset * 2);
    painter.drawRect(rect);
    /*
    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);

    QRectF textRect(adjustedWidth + connectorSize + offset +
    CHEMISTRY_TEXT_SIZE, 0, textSize - offset, h); QTextOption
    textOpt(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter);
    if(m_batteryLevelProvider) {
        painter.drawText(textRect, QString::number((int)(percentage * 100.0f)) +
    "\%", textOpt); } else painter.drawText(textRect, "N/A", textOpt);

    if(m_batteryLevelProvider && charging) {
        painter.drawLine(3.0 * adjustedWidth / 7.0, 4, 3.0 * adjustedWidth
    / 7.0, 6); painter.drawLine(4.0 * adjustedWidth / 7.0, 4, 4.0 *
    adjustedWidth / 7.0, 6); painter.setBrush(Qt::black); painter.drawRect(3.0 *
    adjustedWidth / 7.0 - offset, 6, adjustedWidth / 7.0 + offset * 2, 3);
        painter.drawRect(3.5 * adjustedWidth / 7.0 - 1, 9, 1, 2);
    }
    */
}

void BatteryWidget::constrain() {
    setMinimumSize(45, 14);
    setMaximumSize(45, 14);
}

void BatteryWidget::startTimer() {
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(repaint()));
    timer->start(4000);
}
