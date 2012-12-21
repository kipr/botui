#include "TestWizard.h"
#include "Device.h"
#include "KeyboardDialog.h"
#include "LineEdit.h"

#include "MotorTestWizardPage.h"
#include "ServoTestWizardPage.h"
#include "SensorTestWizardPage.h"
#include "CreateTestWizardPage.h"
#include "NetworkTestWizardPage.h"

#include <QWizardPage>
#include <QLabel>
#include <QVBoxLayout>

TestWizard::TestWizard(Device *device, QWidget *parent)
	: QWizard(parent),
	m_device(device)
{
	setWizardStyle(QWizard::ModernStyle);
	addPage(new MotorTestWizardPage(this));
	addPage(new ServoTestWizardPage(this));
	addPage(new SensorTestWizardPage(this));
	addPage(new CreateTestWizardPage(this));
	addPage(new NetworkTestWizardPage(this));
}