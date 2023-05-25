#ifndef _MOTORTESTWIZARDPAGE_H_
#define _MOTORTESTWIZARDPAGE_H_

#include "OnOffWizardPage.h"

class QTimer;

class MotorTestWizardPage : public OnOffWizardPage {
    Q_OBJECT
  public:
    MotorTestWizardPage(QWidget *parent = 0);

    virtual void initializePage();
    virtual void cleanupPage();

  private slots:
    void motorOn(const quint16 &port);
    void motorOff(const quint16 &port);
    void update();

  private:
    QTimer *m_timer;
    double m_theta;
    bool m_on[4];
};

#endif
