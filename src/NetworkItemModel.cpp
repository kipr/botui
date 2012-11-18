#include "NetworkItemModel.h"
#include "NetworkManager.h"

struct NetworkItem : QStandardItem
{
public:
	NetworkItem(const Network &network)
		: QStandardItem(network.ssid()),
		m_network(network)
	{
		setSizeHint(QSize(32, 32));
		if(m_network.security() != Network::None) {
			setIcon(QIcon(":/tango/network-wireless-encrypted.png"));
		}
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

Network NetworkItemModel::indexToNetwork(const QModelIndex &index) const
{
	QStandardItem *item = itemFromIndex(index);
	if(!item) return Network();
	NetworkItem *networkItem = dynamic_cast<NetworkItem *>(item);
	if(!networkItem) return Network();
	return networkItem->network();
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