#ifndef _MOTORSSENSORSWIDGET_H_
#define _MOTORSSENSORSWIDGET_H_

#include <QWidget>

namespace Ui
{
	class MotorsSensorsWidget;
}

class MenuBar;
class StatusBar;
class Device;

class MotorsSensorsWidget : public QWidget
{
Q_OBJECT
public:
	MotorsSensorsWidget(Device *device, QWidget *parent = 0);
	~MotorsSensorsWidget();
	
public slots:
	void servos();
	void camera();
	
private:
	Ui::MotorsSensorsWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
};

#endif
