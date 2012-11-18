#include "NetworkManager.h"

#include "NMNetworkManager.h"
#include "NMDevice.h"
#include "NMSettings.h"
#include "NMSettingsConnection.h"

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
	connection["connection"]["uuid"] = QUuid::createUuid().toString().remove('{').remove('}');
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
	
	NMSettings settings(NM_SERVICE, NM_OBJECT "/Settings", QDBusConnection::systemBus());
	settings.AddConnection(connection);
}

void NetworkManager::forgetNetwork(const QString& ssid)
{
	NMSettings settings(NM_SERVICE, NM_OBJECT "/Settings", QDBusConnection::systemBus());
	QList<QDBusObjectPath> connections = settings.ListConnections();
	
	foreach(const QDBusObjectPath &connectionPath, connections) {
		NMSettingsConnection conn(NM_SERVICE, connectionPath.path(), QDBusConnection::systemBus());
		Connection details = conn.GetSettings().value();
		
		// This connection is not a wifi one. Skip.
		if(!details.contains("802-11-wireless")) continue;
		
		if(ssid == details["802-11-wireless"]["ssid"].toString()) {
			conn.Delete();
		}
	}
}

NetworkList NetworkManager::networks() const
{
	NetworkList networks;
	NMSettings settings(NM_SERVICE, NM_OBJECT "/Settings", QDBusConnection::systemBus());
	QList<QDBusObjectPath> connections = settings.ListConnections();
	
	// TODO: It would be nice to make this static somewhere
	QMap<QString, Network::Security> securityMap;
	securityMap["none"] = Network::None;
	securityMap["wep"] = Network::Wep;
	securityMap["ieee8021x"] = Network::DynamicWep;
	securityMap["wpa-psk"] = Network::Wpa;
	securityMap["wpa-epa"] = Network::WpaEnterprise;
	
	foreach(const QDBusObjectPath &connectionPath, connections) {
		NMSettingsConnection conn(NM_SERVICE, connectionPath.path(), QDBusConnection::systemBus());
		Connection details = conn.GetSettings().value();
		
		// This connection is not a wifi one. Skip.
		if(!details.contains("802-11-wireless")) continue;
		
		Network network;
		network.setSsid(details["802-11-wireless"]["ssid"].toString());
		network.setSecurity(securityMap[details["802-11-wireless-security"]["security"].toString()]);
		// Technically, password only applies to WEP connections. We always store both password
		// and psk, however, so it is a somewhat safe assumption to only try the password
		// entry.
		network.setPassword(details["802-11-wireless"]["password"].toString());
		networks << network;
	}
	
	return networks;
}

bool NetworkManager::turnOn()
{
	m_nm->Enable(true);
	return true; // TODO: This is a bad assumption
}

void NetworkManager::turnOff()
{
	m_nm->Enable(false);
}

bool NetworkManager::isOn() const
{
	return m_nm->networkingEnabled();
}

NetworkManager::NetworkManager()
	: m_networks(),
	m_nm(new NMNetworkManager(NM_SERVICE, NM_OBJECT,QDBusConnection::systemBus(), this))
{
	// Register our metatype with dbus
	qDBusRegisterMetaType<Connection>();
	
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