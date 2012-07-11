#include "NetworkSettingsWidget.h"
#include "ui_NetworkSettingsWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "ConnectWidget.h"
#include "NetworkingProvider.h"
#include <QDebug>

NetworkSettingsWidget::NetworkSettingsWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::NetworkSettingsWidget),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this))
{
	ui->setupUi(this);
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle("Network Settings");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
	
	QObject::connect(ui->connect, SIGNAL(clicked()), SLOT(connect()));
	QObject::connect(ui->turnOn, SIGNAL(clicked()), SLOT(turnOn()));
	QObject::connect(ui->turnOff, SIGNAL(clicked()), SLOT(turnOff()));
	QObject::connect(m_device->networkingProvider(), SIGNAL(networkStateChanged(NetworkState)), SLOT(updateInformation()));
	
	updateInformation();
}

NetworkSettingsWidget::~NetworkSettingsWidget()
{
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}

void NetworkSettingsWidget::connect()
{
	RootController::ref().presentWidget(new ConnectWidget(m_device));
}

void NetworkSettingsWidget::turnOn()
{
	m_device->networkingProvider()->setNetworkState(NetworkingProvider::NetworkOn);
}

void NetworkSettingsWidget::turnOff()
{
	m_device->networkingProvider()->setNetworkState(NetworkingProvider::NetworkOff);
}

void NetworkSettingsWidget::updateInformation()
{
	setUpdatesEnabled(false);
	NetworkingProvider::NetworkState newState = m_device->networkingProvider()->networkState();
	ui->turnOn->setVisible(false);
	ui->turnOff->setVisible(false);
	const bool networkOn = newState == NetworkingProvider::NetworkOn;
	
	ui->turnOn->setVisible(!networkOn);
	ui->turnOff->setVisible(networkOn);
	
	ui->state->setText(networkOn ? tr("ON") : tr("OFF"));
	ui->ssidLabel->setEnabled(networkOn);
	ui->securityLabel->setEnabled(networkOn);
	ui->security->setEnabled(networkOn);
	
	ui->ssid->setText(networkOn ? "" : "");
	ui->security->setText(networkOn ? "" : "");
	
	
	setUpdatesEnabled(true);
}