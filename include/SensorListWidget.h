#ifndef _SENSORLISTWIDGET_H_
#define _SENSORLISTWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
	class SensorListWidget;
}

class SensorListWidget : public StandardWidget
{
Q_OBJECT
public:
	SensorListWidget(Device *device, QWidget *parent = 0);
	~SensorListWidget();
	
private:
	Ui::SensorListWidget *ui;
};

#endif
