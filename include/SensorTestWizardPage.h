#ifndef _SENSORTESTWIZARDPAGE_H_
#define _SENSORTESTWIZARDPAGE_H_

#include <QWizardPage>
#include "PlotWidget.h"

namespace Ui
{
	class SensorTestWizardPage;
}

class SensorTestWizardPage : public QWizardPage
{
Q_OBJECT
public:
	SensorTestWizardPage(QWidget *parent = 0);
	~SensorTestWizardPage();
	
	void initializePage();
	void cleanupPage();
	
private slots:
	void update();
	
private:
	Ui::SensorTestWizardPage *ui;
	QTimer *m_timer;
	PlotHandle m_plot;
};

#endif
