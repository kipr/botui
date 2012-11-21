#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_

#include "Singleton.h"
#include "Network.h"
#include "NetworkManagerConnection.h"

#include <QObject>
#include <QDBusObjectPath>

class NMNetworkManager;
class NMDeviceWifi;

class NetworkManager : public QObject, public Singleton<NetworkManager>
{
Q_OBJECT
public:
	NetworkManager();
	~NetworkManager();
	
	void addNetwork(const Network &network);
	void forgetNetwork(const Network &network);
	NetworkList networks() const;
	
	Network active() const;
	
	void requestScan();
	
	bool turnOn();
	void turnOff();
	bool isOn() const;
	
	NetworkList accessPoints() const;
	
signals:
	void networkAdded(const Network &network);
	void networkForgotten(const Network &network);
	
	void accessPointAdded(const Network &network);
	void accessPointRemoved(const Network &network);
	
private slots:
	void nmAccessPointAdded(const QDBusObjectPath &path);
	void nmAccessPointRemoved(const QDBusObjectPath &path);
	
private:
	Network networkFromConnection(const Connection &connection) const;
	Network createAccessPoint(const QDBusObjectPath &accessPoint) const;
	
	NetworkList m_accessPoints;
	
	NMNetworkManager *m_nm;
	NMDeviceWifi *m_wifi;
};

#endif
