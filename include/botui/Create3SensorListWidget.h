#ifndef _CREATE3SENSORLISTWIDGET_H_
#define _CREATE3SENSORLISTWIDGET_H_

#include "StandardWidget.h"

#include <QRegularExpression>

namespace Ui
{
	class Create3SensorListWidget;
}

class CreateSensorModel;

class Create3SensorListWidget : public StandardWidget
{
Q_OBJECT
public:
	Create3SensorListWidget(Device *device, QWidget *parent = 0);
	~Create3SensorListWidget();

private slots:
	void coeffChanged();
	void motorChanged();
    int createConnect();
	int isConnected();

	void indexChanged( int index);
	void update();


private:
	Ui::Create3SensorListWidget *ui;

  CreateSensorModel *_model;
	double m_setpointVal;
	double m_feedbackVal;
	int m_position_1;
	double m_vel_1;



};


#endif
