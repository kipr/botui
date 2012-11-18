#ifndef _OTHERNETWORKWIDGET_H_
#define _OTHERNETWORKWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
	class OtherNetworkWidget;
}

class KeyboardDialog;

class OtherNetworkWidget : public StandardWidget
{
Q_OBJECT
public:
	OtherNetworkWidget(Device *device, QWidget *parent = 0);
	~OtherNetworkWidget();
	
private slots:
	void join();
	
private:
	Ui::OtherNetworkWidget *ui;
	
	KeyboardDialog *m_ssid;
	KeyboardDialog *m_password;
};

#endif
