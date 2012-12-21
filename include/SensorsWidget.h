#ifndef _SENSORSWIDGET_H_
#define _SENSORSWIDGET_H_

#include <QWidget>
#include "MenuBar.h"
#include "PlotWidget.h"

namespace Ui
{
	class SensorsWidget;
}

class SensorsWidget : public QWidget
{
Q_OBJECT
public:
	SensorsWidget(Device *device, QWidget *parent = 0);
	~SensorsWidget();
	
public slots:
	void update();
	
private:
	double value(const int &i);
	
	MenuBar *m_menuBar;
	Ui::SensorsWidget *ui;
	PlotHandle m_plots[2];
};

#endif
