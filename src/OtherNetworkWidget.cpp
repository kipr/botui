#include "Options.h"

#ifdef NETWORK_ENABLED

#include "OtherNetworkWidget.h"
#include "ui_OtherNetworkWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "NetworkManager.h"
#include "KeyboardDialog.h"
#include <QDebug>

const static Network::Security securityChoices[] = {
	Network::None,
	Network::Wpa,
	Network::Wep,
	Network::DynamicWep
};

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
	connect(ui->security, SIGNAL(currentIndexChanged(int)),
		SLOT(securityChanged(int)));
	
	securityChanged(0);
}

OtherNetworkWidget::~OtherNetworkWidget()
{
	delete ui;
	
	delete m_ssid;
	delete m_password;
}

void OtherNetworkWidget::fillNetworkInfo(const Network &network)
{
	ui->ssid->setText(network.ssid());
	
	// This does a reverse lookup of the QComboBox index for
	// a given security protocol. Maybe a map in the future?
	int index = 0;
	for(quint16 i = 0; i < 4; ++i) {
		if(securityChoices[i] == network.security()) {
			index = i;
			break;
		}
	}
	ui->security->setCurrentIndex(index);
	ui->password->setText(network.password());
}

void OtherNetworkWidget::join()
{
	Network config;
	config.setSsid(ui->ssid->text());
	config.setSecurity(securityChoices[ui->security->currentIndex()]);
	config.setPassword(ui->password->text());
	NetworkManager::ref().addNetwork(config);
	RootController::ref().dismissWidget();
}

void OtherNetworkWidget::securityChanged(int index)
{
	ui->password->setEnabled(index); // None == 0
}

#endif
