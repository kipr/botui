#ifndef _FIRSTRUNWIZARD_H_
#define _FIRSTRUNWIZARD_H_

#include <QWizard>
#include <QQuickWidget>

class QWizardPage;
class Device;

class FirstRunWizard : public QWizard
{
Q_OBJECT
public:
	FirstRunWizard(Device *device, QQuickWidget *parent = 0);
	
private:
	QWizardPage *createIntroPage();
	QWizardPage *createRegistrationPage();
	QWizardPage *createNamePage();
	QWizardPage *createConclusionPage();
	
	Device *m_device;
};

#endif
