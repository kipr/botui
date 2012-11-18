#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_

#include "Singleton.h"
#include "Network.h"

#include <QObject>

class NMNetworkManager;

class NetworkManager : public QObject, public Singleton<NetworkManager>
{
Q_OBJECT
public:
	NetworkManager();
	~NetworkManager();
	
	void addNetwork(const Network &network);
	void forgetNetwork(const QString& ssid);
	NetworkList networks() const;
	
	bool turnOn();
	void turnOff();
	bool isOn() const;
	
private:
	NetworkList m_networks;
	NMNetworkManager *m_nm;
};

#endif
