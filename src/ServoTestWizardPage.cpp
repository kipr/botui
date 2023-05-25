#include "ServoTestWizardPage.h"

#include <kipr/kipr.h>

#include <QTimer>

#include <cmath>

ServoTestWizardPage::ServoTestWizardPage(QWidget *parent)
    : OnOffWizardPage(parent), m_theta(0.0), m_timer(new QTimer(this)) {
    setTitle(tr("Servo Test"));
    connect(this, SIGNAL(on(quint16)), SLOT(servoOn(quint16)));
    connect(this, SIGNAL(off(quint16)), SLOT(servoOff(quint16)));
}

void ServoTestWizardPage::initializePage() {
    servoOff(0);
    servoOff(1);
    servoOff(2);
    servoOff(3);
    m_timer->start(10);
}

void ServoTestWizardPage::cleanupPage() {
    m_timer->stop();
    servoOff(0);
    servoOff(1);
    servoOff(2);
    servoOff(3);
}

void ServoTestWizardPage::update() {
    m_theta += 0.1;
    const int value = sin(m_theta) * 450 + 512;
    set_servo_position(0, value);
    set_servo_position(1, value);
    set_servo_position(2, value);
    set_servo_position(3, value);
}

void ServoTestWizardPage::servoOn(const quint16 &port) { enable_servo(port); }

void ServoTestWizardPage::servoOff(const quint16 &port) { disable_servo(port); }
