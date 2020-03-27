#include "Options.h"
#include <QCoreApplication>
#ifdef NETWORK_ENABLED

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

NetworkSettingsWidget::NetworkSettingsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::NetworkSettingsWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Network Settings"));

	enableCoolOffTimer = new QTimer(this);
	enableCoolOffTimer->setSingleShot(true);
	QObject::connect(enableCoolOffTimer, SIGNAL(timeout()), SLOT(enableAPControls()));
	
	ui->turnOn->setVisible(false);
	ui->turnOff->setVisible(false);

	QObject::connect(ui->connect, SIGNAL(clicked()), SLOT(connect()));
	QObject::connect(ui->manage, SIGNAL(clicked()), SLOT(manage()));

	//QObject::connect(ui->turnOn, SIGNAL(clicked()), SLOT(disableAPControlsTemporarily()));
	//QObject::connect(ui->turnOff, SIGNAL(clicked()), SLOT(disableAPControlsTemporarily()));
	//NetworkManager::ref().connect(ui->turnOn, SIGNAL(clicked()), SLOT(enableAP())); //SLOT(turnOn()));
	//NetworkManager::ref().connect(ui->turnOff, SIGNAL(clicked()), SLOT(disableAP())); //SLOT(turnOff()));

	QObject::connect(ui->turnOn, SIGNAL(clicked()), SLOT(enableAP()));
	QObject::connect(ui->turnOff, SIGNAL(clicked()), SLOT(disableAP()));
	QObject::connect(ui->tournamentMode, SIGNAL(clicked()), SLOT(TournamentMode()));


	// TODO: put back after we support client mode WiFi
	ui->connect->setVisible(false);
	ui->manage->setVisible(false);
	ui->security->setVisible(false);
	ui->securityLabel->setVisible(false);
	
	QObject::connect(&NetworkManager::ref(),
		SIGNAL(stateChanged(const NetworkManager::State &, const NetworkManager::State &)),
		SLOT(stateChanged(const NetworkManager::State &, const NetworkManager::State &)));
	
	QTimer *updateTimer = new QTimer(this);
	QObject::connect(updateTimer, SIGNAL(timeout()), SLOT(updateInformation()));
	updateTimer->start(10000);

	updateInformation();
}
//Tournament mode code
void NetworkSettingsWidget::TournamentMode(){
	system("sudo iwconfig wlan0 txpower 1");
	QMessageBox msgBox;
	msgBox.setText("Tournament Mode activated");
	msgBox.exec();
}

NetworkSettingsWidget::~NetworkSettingsWidget()
{
	delete ui;
	delete enableCoolOffTimer;
}

void NetworkSettingsWidget::connect()
{
	RootController::ref().presentWidget(new ConnectWidget(device()));
}

void NetworkSettingsWidget::manage()
{
	RootController::ref().presentWidget(new ManageNetworksWidget(device()));
}

void NetworkSettingsWidget::enableAP()
{
	disableAPControlsTemporarily();
	NetworkManager::ref().enableAP();
	ui->turnOn->hide();
	ui->turnOff->show();
}

void NetworkSettingsWidget::disableAP()
{
	disableAPControlsTemporarily();
	NetworkManager::ref().disableAP();
	ui->turnOn->show();
	ui->turnOff->hide();
}

void NetworkSettingsWidget::enableAPControls()
{
	ui->turnOn->setEnabled(true);
	ui->turnOff->setEnabled(true);
}

void NetworkSettingsWidget::disableAPControls()
{
	ui->turnOn->setEnabled(false);
	ui->turnOff->setEnabled(false);
}

void NetworkSettingsWidget::disableAPControlsTemporarily()
{
	ui->turnOn->setEnabled(false);
	ui->turnOff->setEnabled(false);

	enableCoolOffTimer->start(20000);
}

void NetworkSettingsWidget::updateInformation()
{
	const bool on = NetworkStatusWidget::isNetworkUp(); //NetworkManager::ref().isOn();
	ui->state->setText(on ? tr("ON") : tr("OFF"));
	ui->turnOn->setVisible(!on);
	ui->turnOff->setVisible(on);
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
