#include "FirstRunWizard.h"
#include "Device.h"

#include <QWizardPage>
#include <QLabel>
#include <QVBoxLayout>

FirstRunWizard::FirstRunWizard(Device *device, QWidget *parent)
	: QWizard(parent),
	m_device(device)
{
	setWizardStyle(QWizard::ModernStyle);
	addPage(createIntroPage());
	addPage(createRegistrationPage());
}

QWizardPage *FirstRunWizard::createIntroPage()
{
	QWizardPage *page = new QWizardPage();
	page->setTitle("Introduction");

	QLabel *label = new QLabel(tr("This wizard will help you register your %1.").arg(m_device->name()));
	label->setWordWrap(true);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(label);
	page->setLayout(layout);

	return page;
}

QWizardPage *FirstRunWizard::createRegistrationPage()
{
	QWizardPage *page = new QWizardPage();
	page->setTitle("Network Connection");

	QLabel *label = new QLabel(tr("If you have a wireless internet connection, your %1 "
	"can automatically connect and download the latest updates. Would you like to connect to a "
	"wireless network at this time?").arg(m_device->name()));
	label->setWordWrap(true);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(label);
	page->setLayout(layout);

	return page;
}

QWizardPage *FirstRunWizard::createConclusionPage()
{
	return 0;
}