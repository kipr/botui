#include "TimeWidget.h"

#include <QTime>
#include <QTimer>

TimeWidget::TimeWidget(QWidget *parent) : QLabel(parent) {
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start(1000);
    updateTime();
}

void TimeWidget::updateTime() {
    setText(QTime::currentTime().toString("hh:mm:ss A"));
}