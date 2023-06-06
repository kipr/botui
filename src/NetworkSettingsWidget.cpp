#include "Options.h"
#include <QCoreApplication>
#ifdef NETWORK_ENABLED
#include "org_freedesktop_NetworkManager_Connection_Active.h"
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
	: StandardWidget(device, parent), ui(new Ui::NetworkSettingsWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Network Settings"));

	enableCoolOffTimer = new QTimer(this);
	enableCoolOffTimer->setSingleShot(true);

	ui->ConnectButton->setEnabled(false);
	QObject::connect(ui->ConnectButton, SIGNAL(clicked()), SLOT(connect()));
	QObject::connect(ui->ManageButton, SIGNAL(clicked()), SLOT(manage()));

	QObject::connect(ui->TournamentModeButton, SIGNAL(clicked()), SLOT(TournamentMode()));
	NetworkManager::ref().connect(ui->connectionModeSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));

	// TODO: put back after we support client mode WiFi
	ui->ConnectButton->setVisible(true);
	ui->ManageButton->setVisible(true);
	ui->security->setVisible(false);
	ui->securityLabel->setVisible(false);

	QObject::connect(&NetworkManager::ref(),
					 SIGNAL(stateChanged(const NetworkManager::State &, const NetworkManager::State &)),
					 SLOT(stateChanged(const NetworkManager::State &, const NetworkManager::State &)));

	QTimer *updateTimer = new QTimer(this);
	QObject::connect(updateTimer, SIGNAL(timeout()), SLOT(updateInformation()));
	updateTimer->start(10000);

	setupConnectionModeSelect();
	updateInformation();
}

void NetworkSettingsWidget::TournamentMode()
{
	ui->connectionModeSelect->setCurrentIndex(3); // turn wifi off
	msgBox.setText("Tournament Mode activated");
	msgBox.exec();
}

void NetworkSettingsWidget::indexChanged(int index)
{
	if (firstTimeSetup)
	{
		// don't do anything, just setting up the select box
		return;
	}

	NetworkManager::ref().turnOn();

	if (index == 1) // AP mode
	{

		NetworkManager::ref().enableAP();
		ui->ConnectButton->setEnabled(false);
	}
	else if (index == 2) // Wifi on (client mode)
	{

		if (NetworkManager::ref().currentActiveConnectionName() == NetworkManager::ref().getAPName())
		{
			NetworkManager::ref().disableAP();
		}

		ui->ConnectButton->setEnabled(true);
		ui->state->setText("ON");
	}
	else if (index == 3) // Wifi off
	{
		NetworkManager::ref().turnOff();
		ui->ConnectButton->setEnabled(false);
	}
}

NetworkSettingsWidget::~NetworkSettingsWidget()
{
	delete ui;
	delete enableCoolOffTimer;
}

void NetworkSettingsWidget::connect() // Connects to network
{
	RootController::ref().presentWidget(new ConnectWidget(device()));
}

void NetworkSettingsWidget::manage() // Forget or add network to history
{
	RootController::ref().presentWidget(new ManageNetworksWidget(device()));
}

void NetworkSettingsWidget::updateInformation()
{
	const bool on = NetworkManager::ref().isOn();
	// if the wifi device is on, whether we are in AP mode or client, then get the
	// current connection's name (if any) and the current ip address (if any)
	if (on)
	{
		ui->state->setText(tr("ON"));
		ui->ssid->setText(NetworkManager::ref().currentActiveConnectionName());
		ui->ip->setText(NetworkManager::ref().ip4Address());
		ui->password->setText(NetworkManager::ref().activeConnectionPassword());
	}
	// if wifi device is off, say so
	else
	{
		ui->state->setText(tr("OFF"));
		ui->ssid->setText(" ");
		ui->ip->setText(" ");
	}

	Network active = NetworkManager::ref().active();
	ui->security->setText(active.securityString());
}

void NetworkSettingsWidget::stateChanged(const NetworkManager::State &newState, const NetworkManager::State &oldState)
{
	qDebug() << "State Changed to" << newState;
	QTimer::singleShot(300, this, SLOT(updateInformation()));
}

void NetworkSettingsWidget::setupConnectionModeSelect()
{
	firstTimeSetup = true;
	const bool on = NetworkManager::ref().isOn();
	if (!on)
	{
		ui->ConnectButton->setEnabled(false);
		ui->connectionModeSelect->setCurrentIndex(3); // no wifi
	}
	else if (NetworkManager::ref().isActiveConnectionOn())
	{
		if (NetworkManager::ref().isActiveConnectionAP())
		{
			ui->ConnectButton->setEnabled(false);
			ui->connectionModeSelect->setCurrentIndex(1); // ap mode
		}
		else
		{
			ui->ConnectButton->setEnabled(true);
			ui->connectionModeSelect->setCurrentIndex(2); // some other client mode
		}
	}
	else
	{
		// case where wifi is on but it isn't connected to anything yet
		// happens in client mode
		ui->ConnectButton->setEnabled(true);
		ui->connectionModeSelect->setCurrentIndex(2);
	}

	firstTimeSetup = false;
}

#endif
