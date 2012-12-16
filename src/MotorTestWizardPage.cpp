#include "MotorTestWizardPage.h"

#include "kovan/kovan.h"

MotorTestWizardPage::MotorTestWizardPage(QWidget *parent)
	: OnOffWizardPage(parent)
{
	setTitle(tr("Motor Test"));
}

void MotorTestWizardPage::initializePage()
{
	ao();
}

void MotorTestWizardPage::cleanupPage()
{
	ao();
}

void MotorTestWizardPage::motorOn(const quint16 &port)
{
	motor(port, 100);
}

void MotorTestWizardPage::motorOff(const quint16 &port)
{
	motor(port, 0);
}