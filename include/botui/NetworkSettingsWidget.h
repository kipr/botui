#ifndef _NETWORKSETTINGSWIDGET_H_
#define _NETWORKSETTINGSWIDGET_H_

#include "StandardWidget.h"
#include "NetworkManager.h"

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
	void manage();
	void updateInformation();
	
private slots:
	void stateChanged(const NetworkManager::State &newState, const NetworkManager::State &oldState);
	
private:
	Ui::NetworkSettingsWidget *ui;
};

#endif
