#include "OtherNetworkWidget.h"
#include "ui_OtherNetworkWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "NetworkManager.h"
#include "KeyboardDialog.h"
#include <QDebug>

OtherNetworkWidget::OtherNetworkWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::OtherNetworkWidget),
	m_ssid(new KeyboardDialog("Network Name")),
	m_password(new KeyboardDialog("Password"))
{
	ui->setupUi(this);
	performStandardSetup(tr("Other Network"));
	
	ui->ssid->setInputProvider(m_ssid);
	ui->password->setInputProvider(m_password);
	
	connect(ui->join, SIGNAL(clicked()), SLOT(join()));
}

OtherNetworkWidget::~OtherNetworkWidget()
{
	delete ui;
	
	delete m_ssid;
	delete m_password;
}

void OtherNetworkWidget::join()
{
	Network config;
	config.setSsid(m_ssid->input());
	const static Network::Security securityChoices[] = {
		Network::None,
		Network::Wpa,
		Network::Wep,
		Network::DynamicWep
	};
	config.setSecurity(securityChoices[ui->security->currentIndex()]);
	config.setPassword(m_password->input());
	NetworkManager::ref().addNetwork(config);
	RootController::ref().dismissWidget();
}