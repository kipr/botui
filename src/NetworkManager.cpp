#include "NetworkManager.h"
#include "NMNetworkManager.h"
#include "NMDevice.h"
#include "NMSettings.h"

#include <QDBusConnection>

NetworkManager::~NetworkManager()
{
	
}

const NetworkList &NetworkManager::networks() const
{
	return m_networks;
}

#define NM_DEVICE_TYPE_WIFI 2
#define SERVICE "org.freedesktop.NetworkManager"

NetworkManager::NetworkManager()
{
	NMNetworkManager nm(SERVICE,
		"/org/freedesktop/NetworkManager", QDBusConnection::systemBus());
	QList<QDBusObjectPath> objectPaths = nm.GetDevices().value();
	foreach(const QDBusObjectPath &objectPath, objectPaths) {
		qDebug() << "OBJECT:" << objectPath.path();
		NMDevice device(SERVICE, objectPath.path(), QDBusConnection::systemBus());
		if(device.deviceType() == NM_DEVICE_TYPE_WIFI) {
			qDebug() << "\tWIFI!!!";
		}
	}
	
	NMSettings settings(SERVICE,
		"/org/freedesktop/NetworkManager/Settings", QDBusConnection::systemBus());
	QList<QDBusObjectPath> connections = settings.ListConnections();
	foreach(const QDBusObjectPath &connection, connections) {
		qDebug() << "CONNECTION:" << connection.path();
	}
}