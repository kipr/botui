#ifndef _MOTORSSENSORSWIDGET_H_
#define _MOTORSSENSORSWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
	class MotorsSensorsWidget;
}

class MotorsSensorsWidget : public StandardWidget
{
Q_OBJECT
public:
	MotorsSensorsWidget(Device *device, QWidget *parent = 0);
	~MotorsSensorsWidget();
	
public slots:
	void servos();
	void sensors();
	void camera();
	
private:
	Ui::MotorsSensorsWidget *ui;
};

#endif
