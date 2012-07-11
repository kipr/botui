#ifndef _NETWORKSETTINGSWIDGET_H_
#define _NETWORKSETTINGSWIDGET_H_

#include <QWidget>

namespace Ui
{
	class NetworkSettingsWidget;
}

class MenuBar;
class StatusBar;
class Device;

class NetworkSettingsWidget : public QWidget
{
Q_OBJECT
public:
	NetworkSettingsWidget(Device *device, QWidget *parent = 0);
	~NetworkSettingsWidget();
	
public slots:
	void connect();
	void turnOn();
	void turnOff();
	void updateInformation();
	
private:
	Ui::NetworkSettingsWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
};

#endif
