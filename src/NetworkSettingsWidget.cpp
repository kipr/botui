#include "NetworkSettingsWidget.h"
#include "ui_NetworkSettingsWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "ConnectWidget.h"
#include "ManageNetworksWidget.h"
#include "NetworkManager.h"

#include <QTimer>

#include <QDebug>

NetworkSettingsWidget::NetworkSettingsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::NetworkSettingsWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Network Settings"));
	
	ui->turnOn->setVisible(false);
	ui->turnOff->setVisible(false);
	
	QObject::connect(ui->connect, SIGNAL(clicked()), SLOT(connect()));
	QObject::connect(ui->manage, SIGNAL(clicked()), SLOT(manage()));
	NetworkManager::ref().connect(ui->turnOn, SIGNAL(clicked()), SLOT(turnOn()));
	NetworkManager::ref().connect(ui->turnOff, SIGNAL(clicked()), SLOT(turnOff()));
	
	QObject::connect(&NetworkManager::ref(),
		SIGNAL(stateChanged(const NetworkManager::State &, const NetworkManager::State &)),
		SLOT(stateChanged(const NetworkManager::State &, const NetworkManager::State &)));
	
	updateInformation();
}

NetworkSettingsWidget::~NetworkSettingsWidget()
{
	delete ui;
}

void NetworkSettingsWidget::connect()
{
	RootController::ref().presentWidget(new ConnectWidget(device()));
}

void NetworkSettingsWidget::manage()
{
	RootController::ref().presentWidget(new ManageNetworksWidget(device()));
}

void NetworkSettingsWidget::updateInformation()
{
	const bool on = NetworkManager::ref().isOn();
	ui->turnOn->setVisible(!on);
	ui->turnOff->setVisible(on);
	ui->connect->setEnabled(on);
	Network active = NetworkManager::ref().active();
	ui->ssid->setText(active.ssid());
	ui->security->setText(active.securityString());
	const QString ip = NetworkManager::ref().ipAddress();
	ui->ip->setText(ip.isEmpty() ? tr("No IP") : ip);
}

void NetworkSettingsWidget::stateChanged(const NetworkManager::State &newState, const NetworkManager::State &oldState)
{
	qDebug() << "State Changed to" << newState;
	QTimer::singleShot(300, this, SLOT(updateInformation()));
}