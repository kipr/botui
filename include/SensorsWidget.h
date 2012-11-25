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
	
private slots:
	void toggleUi();
	
private:
	MenuBar *m_menuBar;
	QAction m_hideOptions;
	Ui::SensorsWidget *ui;
	PlotHandle m_plots[2];
};

#endif
