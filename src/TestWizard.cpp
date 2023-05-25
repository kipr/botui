#include "TestWizard.h"
#include "Device.h"
#include "KeyboardDialog.h"
#include "LineEdit.h"

#include "CreateTestWizardPage.h"
#include "MotorTestWizardPage.h"
#include "NetworkTestWizardPage.h"
#include "SensorTestWizardPage.h"
#include "ServoTestWizardPage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QWizardPage>

TestWizard::TestWizard(Device *device, QWidget *parent)
    : QWizard(parent), m_device(device) {
    setWizardStyle(QWizard::ModernStyle);
    addPage(new MotorTestWizardPage(this));
    addPage(new ServoTestWizardPage(this));
    addPage(new SensorTestWizardPage(this));
    addPage(new CreateTestWizardPage(this));
#ifdef NETWORK_ENABLED
    addPage(new NetworkTestWizardPage(this));
#endif
}