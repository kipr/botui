#ifndef _NETWORKITEMMODEL_H_
#define _NETWORKITEMMODEL_H_

#include <QStandardItemModel>

#include "Network.h"

class NetworkItemModel : public QStandardItemModel
{
Q_OBJECT
public:
	NetworkItemModel(QObject *parent = 0);
	~NetworkItemModel();
	
	Network indexToNetwork(const QModelIndex &index) const;
	
	void setNetworks(const NetworkList &networks);
	
public slots:
	void networkAdded(const Network &network);
	void networkRemoved(const Network &network);
};

#endif
