#ifndef _CREATE3SENSORLISTWIDGET_H_
#define _CREATE3SENSORLISTWIDGET_H_

#include "StandardWidget.h"

#include <QRegularExpression>

namespace Ui
{
	class Create3SensorListWidget;
}

class Create3SensorModel;

class Create3SensorListWidget : public StandardWidget
{
	Q_OBJECT
public:
	Create3SensorListWidget(Device *device, QWidget *parent = 0);
	~Create3SensorListWidget();

private slots:


private:
	Ui::Create3SensorListWidget *ui;
	Create3SensorModel *_model;
	
};

#endif
