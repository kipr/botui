#ifndef _NETWORKSETTINGSWIDGET_H_
#define _NETWORKSETTINGSWIDGET_H_

#include "Options.h"

#ifdef NETWORK_ENABLED

#include "StandardWidget.h"
#include "NetworkManager.h"

class QTimer;

namespace Ui
{
	class NetworkSettingsWidget;
}

class NetworkSettingsWidget : public StandardWidget
{
Q_OBJECT
public:
	enum Mode : uint8_t
	{
		Normal = 0,
		Tournament = 1,
		Unknown = 2
	};

	NetworkSettingsWidget(Device *device, QWidget *parent = 0);
	~NetworkSettingsWidget();

	static void setMode(const Mode mode);
	static Mode mode();
	
public slots:
	void connect();
	void manage();
	void updateInformation();
	void enableAP();
	void disableAP();
	void enableAPControls();
	void disableAPControls();
	void disableAPControlsTemporarily();

	
private slots:
	void stateChanged(const NetworkManager::State &newState, const NetworkManager::State &oldState);

private:
	int currentMode();

	Ui::NetworkSettingsWidget *ui;
	QTimer *enableCoolOffTimer;
	QProcess proc;
	QMessageBox msgBox;

	static Mode mode_;
};

#endif

#endif
