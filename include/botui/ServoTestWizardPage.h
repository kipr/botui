#ifndef _SERVOTESTWIZARDPAGE_H_
#define _SERVOTESTWIZARDPAGE_H_

#include "OnOffWizardPage.h"

class QTimer;

class ServoTestWizardPage : public OnOffWizardPage {
    Q_OBJECT
  public:
    ServoTestWizardPage(QWidget *parent = 0);

    virtual void initializePage();
    virtual void cleanupPage();

  private slots:
    void update();

    void servoOn(const quint16 &port);
    void servoOff(const quint16 &port);

  private:
    double m_theta;
    QTimer *m_timer;
};

#endif
