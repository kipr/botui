#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_

#include "Singleton.h"
#include "Network.h"

#include <QObject>

class NetworkManager : public QObject, public Singleton<NetworkManager>
{
Q_OBJECT
public:
	NetworkManager();
	~NetworkManager();
	
	const NetworkList &networks() const;
	
	bool connect();
	void disconnect();
	bool isConnected() const;
	
private:
	
	NetworkList m_networks;
};

#endif
