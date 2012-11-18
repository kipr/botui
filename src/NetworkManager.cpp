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
	// Make IPv4 automatic
	QVariantMap ip4;
	ip4["method"] = "auto";
	
	// Turn off IPv6
	QVariantMap ip6;
	ip6["method"] = "ignore";
	
	QVariantMap connection;
	// For now we only handle wifi
	connection["type"] = "802-11-wireless";
	connection["uuid"] = QUuid::createUuid().toString();
	// File name is just the SSID for now
	connection["id"] = network.ssid();
	
	const static char *securityTypes[] = {
		"none",
		"wep",
		"ieee8021x",
		"wpa-psk",
		"wpa-epa"
	};
	
	QVariantMap wifi;
	wifi["ssid"] = network.ssid().toLatin1();
	QVariantMap securityConfig;
	if(network.security() != Network::None) {
		wifi["security"] = "802-11-wireless-security";
		securityConfig["key-mgmt"] = securityTypes[network.security()];
		
		// WEP uses this key
		securityConfig["password"] = network.password();
		
		// WPA uses this one
		securityConfig["psk"] = network.password();
	}
	
	QVariantMap config;
	config["connection"] = connection;
	config["802-11-wireless"] = wifi;
	if(!securityConfig.isEmpty()) {
		config["802-11-wireless-security"] = securityConfig;
	}
	config["ipv4"] = ip4;
	config["ipv6"] = ip6;
	
	NMSettings settings(NM_SERVICE,
		"/org/freedesktop/NetworkManager/Settings", QDBusConnection::systemBus());
	settings.AddConnection(config);
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