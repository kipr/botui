#include "NetworkManager.h"

#include "NMNetworkManager.h"
#include "NMDevice.h"
#include "NMSettings.h"

#include <QDBusConnection>
#include <QUuid>

#define NM_DEVICE_TYPE_WIFI 2
#define NM_SERVICE "org.freedesktop.NetworkManager"
#define NM_OBJECT "/org/freedesktop/NetworkManager"

NetworkManager::~NetworkManager()
{
	
}

void NetworkManager::addNetwork(const Network &network)
{
	Connection connection;
	connection["ipv4"]["method"] = "auto";
	connection["ipv6"]["method"] = "auto";
	
	// For now we only handle wifi
	connection["connection"]["type"] = "802-11-wireless";
	connection["connection"]["uuid"] = QUuid::createUuid().toString();
	// File name is just the SSID for now
	connection["connection"]["id"] = network.ssid();
	
	const static QString securityTypes[] = {
		"none",
		"wep",
		"ieee8021x",
		"wpa-psk",
		"wpa-epa"
	};
	
	connection["802-11-wireless"]["ssid"] = network.ssid().toLatin1();
	if(network.security() != Network::None) {
		connection["802-11-wireless-security"]["security"] = securityTypes[network.security()];
		// WEP uses this key
		connection["802-11-wireless-security"]["password"] = network.password();
		// WPA uses this one
		connection["802-11-wireless-security"]["psk"] = network.password();
	}
	
	NMSettings settings(NM_SERVICE,
		"/org/freedesktop/NetworkManager/Settings", QDBusConnection::systemBus());
	settings.AddConnection(connection);
}

const NetworkList &NetworkManager::networks() const
{
	return m_networks;
}

NetworkManager::NetworkManager()
	: m_networks(),
	m_nm(new NMNetworkManager(NM_SERVICE, NM_OBJECT,QDBusConnection::systemBus(), this))
{
	QList<QDBusObjectPath> objectPaths = m_nm->GetDevices().value();
	foreach(const QDBusObjectPath &objectPath, objectPaths) {
		qDebug() << "OBJECT:" << objectPath.path();
		NMDevice device(NM_SERVICE, objectPath.path(), QDBusConnection::systemBus());
		if(device.deviceType() == NM_DEVICE_TYPE_WIFI) {
			qDebug() << "\tWIFI!!!";
		}
	}
	
	NMSettings settings(NM_SERVICE,
		"/org/freedesktop/NetworkManager/Settings", QDBusConnection::systemBus());
	QList<QDBusObjectPath> connections = settings.ListConnections();
	foreach(const QDBusObjectPath &connection, connections) {
		qDebug() << "CONNECTION:" << connection.path();
	}
}