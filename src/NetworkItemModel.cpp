#include "Options.h"

#ifdef NETWORK_ENABLED

#include "NetworkItemModel.h"
#include "NetworkManager.h"

struct NetworkItem : QStandardItem {
  public:
    NetworkItem(const Network &network)
        : QStandardItem(network.ssid()), m_network(network) {
        setSizeHint(QSize(32, 32));
        if (m_network.security() != Network::None) {
            setIcon(QIcon(":/tango/network-wireless-encrypted.png"));
        }
    }

    const Network &network() const { return m_network; }

  private:
    Network m_network;
};

NetworkItemModel::NetworkItemModel(QObject *parent)
    : QStandardItemModel(parent) {}

NetworkItemModel::~NetworkItemModel() {}

Network NetworkItemModel::indexToNetwork(const QModelIndex &index) const {
    QStandardItem *item = itemFromIndex(index);
    if (!item) return Network();
    NetworkItem *networkItem = dynamic_cast<NetworkItem *>(item);
    if (!networkItem) return Network();
    return networkItem->network();
}

void NetworkItemModel::setNetworks(const NetworkList &networks) {
    clear();
    foreach (const Network &network, networks)
        addNetwork(network);
}

void NetworkItemModel::addNetwork(const Network &network) {
    appendRow(new NetworkItem(network));
}

void NetworkItemModel::removeNetwork(const Network &network) {
    for (int i = 0; i < rowCount(); ++i) {
        NetworkItem *networkItem = dynamic_cast<NetworkItem *>(item(i));
        if (network == networkItem->network()) qDeleteAll(takeRow(i));
    }
}

#endif