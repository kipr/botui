#ifndef _NETWORKSETTINGSWIDGET_H_
#define _NETWORKSETTINGSWIDGET_H_

#include "Options.h"

#ifdef NETWORK_ENABLED

#include "StandardWidget.h"
#include "NetworkManager.h"

#include <QProcess>
#include <QMessageBox>

class QTimer;

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
	QString getRaspberryPiType();
	void getWombatName();
	void editWifiConnectionMode(int newMode);
	void eventModeEnabledState();
	void eventModeDisabledState();
	void getCurrentMode();
	QString getConnectionConfig();

public slots:
	void connect();
	void manage();
	void updateInformation();
	
private slots:
	void stateChanged(const NetworkManager::State &newState, const NetworkManager::State &oldState);
	void indexChanged(int index);
	void toggleChanged();
	void rebootBox();
	 void onStateChanged(const QString &oldBand, const QString &newBand, int oldChannel, int newChannel);
private:
	Ui::NetworkSettingsWidget *ui;
	QTimer *enableCoolOffTimer;
	QProcess proc;
	QMessageBox *msgBox;
};

#endif

#endif
