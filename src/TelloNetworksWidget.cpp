

#include "TelloNetworksWidget.h"
#include "ui_TelloNetworksWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "TelloWidget.h"
#include "NetworkManager.h"
#include "NetworkItemModel.h"
#include "OtherNetworkWidget.h"
#include "Device.h"
#include <QDebug>
#include "Options.h"

const static Network::Security securityChoices[] = {
	Network::None,
	Network::Wpa,
	Network::Wep,
	Network::DynamicWep
};

TelloNetworksWidget::TelloNetworksWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::TelloNetworksWidget),
	m_model(new NetworkItemModel(this))
{
	
	ui->setupUi(this);
	performStandardSetup(tr("Tello Networks Widget"));
	
	m_model->setNetworks(NetworkManager::ref().telloAccessPoints());
	
	ui->networks->setModel(m_model);
	ui->connect->setEnabled(true);
	m_model->connect(&NetworkManager::ref(), SIGNAL(accessPointAdded(Network)),
		SLOT(addNetwork(Network)));
	m_model->connect(&NetworkManager::ref(), SIGNAL(accessPointRemoved(Network)),
		SLOT(removeNetwork(Network)));
	
	QObject::connect(ui->refresh, SIGNAL(clicked()), SLOT(refresh()));
	QObject::connect(ui->other, SIGNAL(clicked()), SLOT(other()));
	QObject::connect(ui->connect, SIGNAL(clicked()), SLOT(connect()));
	QObject::connect(ui->networks->selectionModel(),
		SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
		SLOT(selectionChanged(QItemSelection)));
	selectionChanged(QItemSelection());

}

TelloNetworksWidget::~TelloNetworksWidget()
{
	delete ui;
}

void TelloNetworksWidget::connect()
{
	QModelIndexList selection = ui->networks->selectionModel()->selectedIndexes();
	if(selection.size() != 1) return;
	Network network = m_model->indexToNetwork(selection[0]);

    qDebug() << "Tello Network selected: " << network.ssid();
    Network configs;
	configs.setSsid(network.ssid());
	configs.setMode(Network::Infrastructure);
    configs.setSecurity(securityChoices[0]);
	foreach(const Network &nw, NetworkManager::ref().telloAccessPoints())
	{
		if(nw.ssid() == network.ssid()){
			configs.setAPPath(nw.apPath());
		}
	}
	NetworkManager::ref().addNetwork(configs);
    RootController::ref().presentWidget(new TelloWidget(device()));

    
}


void TelloNetworksWidget::refresh()
{
	NetworkManager::ref().requestScan();
}

void TelloNetworksWidget::selectionChanged(const QItemSelection &selection)
{
	ui->connect->setEnabled(selection.indexes().size());
}

