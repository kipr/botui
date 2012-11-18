#include "NetworkManager.h"
#include "NMNetworkManager.h"

#include <QDBusConnection>

NetworkManager::~NetworkManager()
{
	
}

const NetworkList &NetworkManager::networks() const
{
	return m_networks;
}

NetworkManager::NetworkManager()
{
	org::freedesktop::NetworkManager nm("org.freedesktop.NetworkManager",
		"/org/freedesktop/NetworkManager", QDBusConnection::systemBus());
	QList<QDBusObjectPath> objectPaths = nm.GetDevices().value();
	foreach(const QDBusObjectPath &objectPath, objectPaths) {
		qDebug() << "OBJECT:" << objectPath.path();
	}
}