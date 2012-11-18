#include "NetworkItemModel.h"
#include "NetworkManager.h"

struct NetworkItem : QStandardItem
{
public:
	NetworkItem(const Network &network)
		: QStandardItem(network.ssid()),
		m_network(network)
	{
	}
	
	const Network &network() const
	{
		return m_network;
	}
	
private:
	Network m_network;
};

NetworkItemModel::NetworkItemModel(QObject *parent)
	: QStandardItemModel(parent)
{
}

NetworkItemModel::~NetworkItemModel()
{
}

void NetworkItemModel::setNetworks(const NetworkList &networks)
{
	clear();
	foreach(const Network &network, networks) networkAdded(network);
}

void NetworkItemModel::networkAdded(const Network &network)
{
	appendRow(new NetworkItem(network));
}

void NetworkItemModel::networkRemoved(const Network &network)
{
	// FIXME: NYI
}