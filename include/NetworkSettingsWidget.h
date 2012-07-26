#ifndef _NETWORKSETTINGSWIDGET_H_
#define _NETWORKSETTINGSWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
	class NetworkSettingsWidget;
}

class NetworkSettingsWidget : public StandardWidget
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
};

#endif
