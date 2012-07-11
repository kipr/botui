#ifndef _HOMEWIDGET_H_
#define _HOMEWIDGET_H_

#include <QWidget>

namespace Ui
{
	class HomeWidget;
}

class MenuBar;
class StatusBar;
class Device;

class HomeWidget : public QWidget
{
Q_OBJECT
public:
	HomeWidget(Device *device, QWidget *parent = 0);
	~HomeWidget();

public slots:
	void programs();
	void motorsSensors();
	void settings();
	void about();
	void lock();
	
private:
	Ui::HomeWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
};


#endif
