#include "Options.h"

#ifdef NETWORK_ENABLED

#include "Config.h"
#include "NetworkSettingsWidget.h"
#include "NetworkStatusWidget.h"
#include "ui_NetworkSettingsWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "ConnectWidget.h"
#include "ManageNetworksWidget.h"
#include "NetworkManager.h"
#include "SystemUtils.h"
#include "Execute.h"

#include <QTimer>

#include <QDebug>
#include <fstream>
#include <sstream>

NetworkSettingsWidget::Mode NetworkSettingsWidget::mode_ = NetworkSettingsWidget::Unknown;

namespace
{
	void limitTxPower()
	{
		system("sudo iwconfig wlan0 txpower 1");
	}

	void autoTxPower()
	{
		system("sudo iwconfig wlan0 txpower 1496");
	}
}

NetworkSettingsWidget::NetworkSettingsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::NetworkSettingsWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Network Settings"));

	enableCoolOffTimer = new QTimer(this);
	enableCoolOffTimer->setSingleShot(true);
	QObject::connect(enableCoolOffTimer, SIGNAL(timeout()), SLOT(enableAPControls()));
	
	QObject::connect(ui->connect, SIGNAL(clicked()), SLOT(connect()));
	QObject::connect(ui->manage, SIGNAL(clicked()), SLOT(manage()));

	//QObject::connect(ui->turnOn, SIGNAL(clicked()), SLOT(disableAPControlsTemporarily()));
	//QObject::connect(ui->turnOff, SIGNAL(clicked()), SLOT(disableAPControlsTemporarily()));
	//NetworkManager::ref().connect(ui->turnOn, SIGNAL(clicked()), SLOT(enableAP())); //SLOT(turnOn()));
	//NetworkManager::ref().connect(ui->turnOff, SIGNAL(clicked()), SLOT(disableAP())); //SLOT(turnOff()));

	QObject::connect(ui->mode, SIGNAL(currentIndexChanged(int)), SLOT(modeChanged(int)));

	// TODO: put back after we support client mode WiFi
	ui->connect->setVisible(false);
	ui->manage->setVisible(false);
	ui->security->setVisible(false);
	ui->securityLabel->setVisible(false);

	ui->mode->setCurrentIndex(NetworkSettingsWidget::mode() == NetworkSettingsWidget::Normal
		? 0
		: 1
	);
	
	QObject::connect(&NetworkManager::ref(),
		SIGNAL(stateChanged(const NetworkManager::State &, const NetworkManager::State &)),
		SLOT(stateChanged(const NetworkManager::State &, const NetworkManager::State &)));
	
	QTimer *updateTimer = new QTimer(this);
	QObject::connect(updateTimer, SIGNAL(timeout()), SLOT(updateInformation()));
	updateTimer->start(10000);

	updateInformation();
}

NetworkSettingsWidget::~NetworkSettingsWidget()
{
	delete ui;
	delete enableCoolOffTimer;
}

void NetworkSettingsWidget::setMode(const NetworkSettingsWidget::Mode mode)
{
	std::ostringstream o;
	o << botui::pathToKISS.toStdString();
	o << "network_settings";

	mode_ = mode;

	std::ofstream f(o.str());
	if (f.is_open())
	{
		f.write(reinterpret_cast<const char *>(&mode_), sizeof (mode_));
		f.close();
	}
	else
	{
		qWarning() << "Error opening " << o.str().c_str() << " for writing";
	}
	
	switch (mode_)
	{
		case NetworkSettingsWidget::Normal:
			autoTxPower();
			break;
		case NetworkSettingsWidget::Tournament:
			limitTxPower();
			break;
	}
}


NetworkSettingsWidget::Mode NetworkSettingsWidget::mode()
{
	if (mode_ == NetworkSettingsWidget::Unknown)
	{
		std::ostringstream o;
		o << botui::pathToKISS.toStdString();
		o << "network_settings";

		qWarning() << o.str().c_str();

		std::ifstream f(o.str());
		if (!f)
		{
			qWarning() << "Failure to open network_settings file";
			return NetworkSettingsWidget::Normal;
		}

		f.read(reinterpret_cast<char *>(&mode_), sizeof (mode_));
		f.close();
	}
	
	return mode_;
}

void NetworkSettingsWidget::connect()
{
	RootController::ref().presentWidget(new ConnectWidget(device()));
}

void NetworkSettingsWidget::manage()
{
	RootController::ref().presentWidget(new ManageNetworksWidget(device()));
}

void NetworkSettingsWidget::modeChanged(int index)
{
	NetworkSettingsWidget::setMode(index == 0
		? NetworkSettingsWidget::Normal
		: NetworkSettingsWidget::Tournament
	);
}

int NetworkSettingsWidget::currentMode()
{
	// Determine appropriate index
	QProcess process;
	process.start("iwconfig", QStringList() << QString("wlan0"));
	process.waitForFinished();
	QByteArray out = process.readAllStandardOutput();
	qWarning() << "iwconfig output: " << out;
}

void NetworkSettingsWidget::enableAP()
{
	disableAPControlsTemporarily();
	NetworkManager::ref().enableAP();
}

void NetworkSettingsWidget::disableAP()
{
	disableAPControlsTemporarily();
	NetworkManager::ref().disableAP();
}

void NetworkSettingsWidget::enableAPControls()
{
}

void NetworkSettingsWidget::disableAPControls()
{
}

void NetworkSettingsWidget::disableAPControlsTemporarily()
{
	enableCoolOffTimer->start(20000);
}

void NetworkSettingsWidget::updateInformation()
{
	const bool on = NetworkStatusWidget::isNetworkUp(); //NetworkManager::ref().isOn();
	ui->state->setText(on ? tr("ON") : tr("OFF"));
	ui->connect->setEnabled(on);

	const QString id = device()->id();
	const QString serial = device()->serial();
	if(!id.isEmpty()) {
		const QString password = SystemUtils::sha256(id).left(6) + "00";
                const QString ssid = serial + "-wombat";
		ui->ssid->setText(ssid);
		ui->password->setText(password);
	}

	Network active = NetworkManager::ref().active();
	//ui->ssid->setText(active.ssid());
	ui->security->setText(active.securityString());
	const QString ip = NetworkManager::ref().ipAddress();
	ui->ip->setText(ip.isEmpty() ? tr("No IP") : ip);
}

void NetworkSettingsWidget::stateChanged(const NetworkManager::State &newState, const NetworkManager::State &oldState)
{
	qDebug() << "State Changed to" << newState;
	QTimer::singleShot(300, this, SLOT(updateInformation()));
}

#endif
