#ifndef _NETWORKINGPROVIDER_H_
#define _NETWORKINGPROVIDER_H_

#include <QObject>

class QAbstractItemModel;

class NetworkingProvider : public QObject
{
Q_OBJECT
Q_PROPERTY(NetworkState networkState
	READ networkState
	WRITE setNetworkState
	NOTIFY networkStateChanged)

public:
	virtual ~NetworkingProvider();
	
	enum NetworkState {
		NetworkUnknown,
		NetworkOn,
		NetworkOff
	};
	
	virtual const NetworkingProvider::NetworkState networkState() const = 0;
	virtual void setNetworkState(const NetworkingProvider::NetworkState& state) = 0;
	virtual QAbstractItemModel *networkItemModel() const = 0;
	
	virtual const float networkStrength() const = 0;
	virtual const float networkStrengthMin() const = 0;
	virtual const float networkStrengthMax() const = 0;

signals:
	void networkStateChanged(const NetworkState& state);
	void connected(const QString& network);
};

#endif
