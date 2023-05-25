#ifndef _TESTWIZARD_H_
#define _TESTWIZARD_H_

#include <QWizard>

class QWizardPage;
class Device;

class TestWizard : public QWizard {
    Q_OBJECT
  public:
    TestWizard(Device *device, QWidget *parent = 0);

  private:
    Device *m_device;
};

#endif
