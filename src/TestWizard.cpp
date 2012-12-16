#include "TestWizard.h"
#include "Device.h"
#include "KeyboardDialog.h"
#include "LineEdit.h"

#include <QWizardPage>
#include <QLabel>
#include <QVBoxLayout>

TestWizard::TestWizard(Device *device, QWidget *parent)
	: QWizard(parent),
	m_device(device)
{
	setWizardStyle(QWizard::ModernStyle);
}