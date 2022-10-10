#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_

#include "Options.h"

#ifdef NETWORK_ENABLED

#include "Singleton.h"
#include "Network.h"
#include "NetworkManagerConnection.h"

#include <QObject>

#include <QDBusObjectPath>

class NMNetworkManager;
class NMDevice;
class NMDeviceWifi;

class NetworkManager : public QObject, public Singleton<NetworkManager>
{
Q_OBJECT
public:
	enum State {
		Unknown = 0,
		Unmanaged = 10,
		Unavailable = 20,
		Disconnected = 30,
		Prepare = 40,
		Config = 50,
		NeedAuth = 60,
		IpConfig = 70,
		IpCheck = 80,
		Secondaries = 90,
		Activated = 100,
		Deactivating = 110,
		Failed = 120
	};
	
	NetworkManager();
	~NetworkManager();
	
	void addNetwork(const Network &network);
	void forgetNetwork(const Network &network);
	NetworkList networks() const;
	
	Network active() const;
	
	void requestScan();
	
	bool isOn() const;
	bool isPersistentOn() const;
	
	State state() const;
	
	NetworkList accessPoints() const;
	
	QString ipAddress() const;
	
public slots:
	bool turnOn();
	void turnOff();
	
	bool enableAP();
	bool disableAP();


signals:
	void networkAdded(const Network &network);
	void networkForgotten(const Network &network);
	
	void accessPointAdded(const Network &network);
	void accessPointRemoved(const Network &network);
	
	void stateChanged(const NetworkManager::State &newState, const NetworkManager::State &oldState);
	
private slots:
	void nmAccessPointAdded(const QDBusObjectPath &path);
	void nmAccessPointRemoved(const QDBusObjectPath &path);
	
	void stateChangedBouncer(uint newState, uint oldState);
	
private:
	Network networkFromConnection(const Connection &connection) const;
	Network createAccessPoint(const QDBusObjectPath &accessPoint) const;
	
	NetworkList m_accessPoints;
	
	NMNetworkManager *m_nm;
	NMDevice *m_device;
	NMDeviceWifi *m_wifi;
};

#endif

#endif
