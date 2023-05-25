#include "Options.h"

#ifdef NETWORK_ENABLED

#ifndef _NETWORKITEMMODEL_H_
#define _NETWORKITEMMODEL_H_

#include <QStandardItemModel>

#include "Network.h"

class NetworkItemModel : public QStandardItemModel {
    Q_OBJECT
  public:
    NetworkItemModel(QObject *parent = 0);
    ~NetworkItemModel();

    Network indexToNetwork(const QModelIndex &index) const;

    void setNetworks(const NetworkList &networks);

  public slots:
    void addNetwork(const Network &network);
    void removeNetwork(const Network &network);
};

#endif

#endif