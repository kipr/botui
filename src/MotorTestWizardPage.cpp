#include "MotorTestWizardPage.h"
#include "ArrayHelpers.h"

#include <kipr/kipr.h>

#include <QTimer>

#include <cmath>

MotorTestWizardPage::MotorTestWizardPage(QWidget *parent)
    : OnOffWizardPage(parent), m_timer(new QTimer(this)), m_theta(0.0) {
    memset(m_on, false, sizeof_array(m_on) * sizeof(bool));
    setTitle(tr("Motor Test"));
    connect(this, SIGNAL(on(quint16)), SLOT(motorOn(quint16)));
    connect(this, SIGNAL(off(quint16)), SLOT(motorOff(quint16)));
}

void MotorTestWizardPage::initializePage() {
    m_timer->start(10);
    ao();
}

void MotorTestWizardPage::cleanupPage() {
    m_timer->stop();
    ao();
}

void MotorTestWizardPage::motorOn(const quint16 &port) { m_on[port] = true; }

void MotorTestWizardPage::motorOff(const quint16 &port) { m_on[port] = false; }

void MotorTestWizardPage::update() {
    m_theta += 0.1;
    const int value = sin(m_theta) * 100;
    for (int i = 0; i < 4; ++i) {
        if (m_on[i]) motor(i, value);
    }
}
