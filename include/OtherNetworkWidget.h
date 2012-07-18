#ifndef _OTHERNETWORKWIDGET_H_
#define _OTHERNETWORKWIDGET_H_

#include <QWidget>

namespace Ui
{
	class OtherNetworkWidget;
}

class MenuBar;
class StatusBar;
class Device;
class KeyboardDialog;

class OtherNetworkWidget : public QWidget
{
Q_OBJECT
public:
	OtherNetworkWidget(Device *device, QWidget *parent = 0);
	~OtherNetworkWidget();
	
private:
	Ui::OtherNetworkWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
	
	KeyboardDialog *m_ssid;
	KeyboardDialog *m_password;
};

#endif
