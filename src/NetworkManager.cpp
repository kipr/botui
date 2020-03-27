#include "Options.h"

#ifdef NETWORK_ENABLED

#include "NetworkManager.h"

#include "NMNetworkManager.h"
#include "NMAccessPoint.h"
#include "NMDevice.h"
#include "NMDeviceWifi.h"
#include "NMSettings.h"
#include "NMSettingsConnection.h"

#include <stdio.h>

#include <string.h> /* for strncpy */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <QDBusConnection>
#include <QUuid>

// Applicable defines from the NetworkManager DBus spec

#define NM_DEVICE_TYPE_WIFI 2

// Null flag.
#define NM_802_11_AP_SEC_NONE 0x0
// Access point supports pairwise 40-bit WEP encryption.
#define NM_802_11_AP_SEC_PAIR_WEP40 0x1
// Access point supports pairwise 104-bit WEP encryption.
#define NM_802_11_AP_SEC_PAIR_WEP104 0x2
// Access point supports pairwise TKIP encryption.
#define NM_802_11_AP_SEC_PAIR_TKIP 0x4
// Access point supports pairwise CCMP encryption.
#define NM_802_11_AP_SEC_PAIR_CCMP 0x8
// Access point supports a group 40-bit WEP cipher.
#define NM_802_11_AP_SEC_GROUP_WEP40 0x10
// Access point supports a group 104-bit WEP cipher.
#define NM_802_11_AP_SEC_GROUP_WEP104 0x20
// Access point supports a group TKIP cipher.
#define NM_802_11_AP_SEC_GROUP_TKIP 0x40
// Access point supports a group CCMP cipher.
#define NM_802_11_AP_SEC_GROUP_CCMP 0x80
// Access point supports PSK key management.
#define NM_802_11_AP_SEC_KEY_MGMT_PSK 0x100
// Access point supports 802.1x key management.
#define NM_802_11_AP_SEC_KEY_MGMT_802_1X 0x200

// Mode is unknown.
#define NM_802_11_MODE_UNKNOWN 0

// Uncoordinated network without central infrastructure.
#define NM_802_11_MODE_ADHOC 1
// Coordinated network with one or more central controllers.
#define NM_802_11_MODE_INFRA 2

#define NM_SERVICE "org.freedesktop.NetworkManager"
#define NM_OBJECT "/org/freedesktop/NetworkManager"

NetworkManager::~NetworkManager()
{
	
}

#define NM_802_11_WIRELESS_KEY ("802-11-wireless")
#define NM_802_11_SECURITY_KEY ("802-11-wireless-security")

void NetworkManager::addNetwork(const Network &network)
{
	// Yes, yes... this is a hard coded mess.
	// Maybe this should be generalized in the future.
	
	Connection connection;
	connection["ipv4"]["method"] = "auto";
	connection["ipv6"]["method"] = "auto";
	
	// For now we only handle wifi
	connection["connection"]["type"] = "802-11-wireless";
	connection["connection"]["uuid"] = QUuid::createUuid().toString().remove('{').remove('}');
	// File name is just the SSID for now
	connection["connection"]["id"] = network.ssid();
	
	// SSID
	connection[NM_802_11_WIRELESS_KEY]["ssid"] = network.ssid().toLatin1();
	
	// Network Mode (adhoc or infrastructure)
	switch(network.mode()) {
	case Network::Infrastructure:
		connection[NM_802_11_WIRELESS_KEY]["mode"] = "infrastructure";
		break;
	case Network::AdHoc:
		connection[NM_802_11_WIRELESS_KEY]["mode"] = "adhoc";
		break;
	default: break;
	}
	
	const static QString securityTypes[] = {
		"none",
		"wep",
		"ieee8021x",
		"wpa-psk",
		"wpa-epa"
	};
	
	if(network.security() != Network::None) {
		connection[NM_802_11_SECURITY_KEY]["key-mgmt"] = securityTypes[network.security()];
		// WEP uses this key
		connection[NM_802_11_SECURITY_KEY]["password"] = network.password();
		// WPA uses this one
		connection[NM_802_11_SECURITY_KEY]["psk"] = network.password();
		
		// Finally, tell our configuration about the security
		connection[NM_802_11_WIRELESS_KEY]["security"] = NM_802_11_SECURITY_KEY;
	}
	
	// Send our config via dbus to NetworkManager
	NMSettings settings(NM_SERVICE, NM_OBJECT "/Settings", QDBusConnection::systemBus());
	settings.AddConnection(connection);
	emit networkAdded(network);
}

void NetworkManager::forgetNetwork(const Network &network)
{
	NMSettings settings(NM_SERVICE, NM_OBJECT "/Settings", QDBusConnection::systemBus());
	QList<QDBusObjectPath> connections = settings.ListConnections();
	
	foreach(const QDBusObjectPath &connectionPath, connections) {
		NMSettingsConnection conn(NM_SERVICE, connectionPath.path(), QDBusConnection::systemBus());
		Connection details = conn.GetSettings().value();
		
		// This connection is not a wifi one. Skip.
		if(!details.contains("802-11-wireless")) continue;
		
		if(network.ssid() == details["802-11-wireless"]["ssid"].toString()) {
			conn.Delete();
		}
	}
	
	emit networkForgotten(network);
}

NetworkList NetworkManager::networks() const
{
	NetworkList networks;
	NMSettings settings(NM_SERVICE, NM_OBJECT "/Settings", QDBusConnection::systemBus());
	QList<QDBusObjectPath> connections = settings.ListConnections();
	
	foreach(const QDBusObjectPath &connectionPath, connections) {
		NMSettingsConnection conn(NM_SERVICE, connectionPath.path(), QDBusConnection::systemBus());
		Connection details = conn.GetSettings().value();
		// This connection is not a wifi one. Skip.
		if(!details.contains("802-11-wireless")) continue;
		networks << networkFromConnection(details);
	}
	
	return networks;
}

void NetworkManager::requestScan()
{
	if(!m_wifi) return;
	qDebug() << "Requesting scan";
	QDBusPendingReply<> reply = m_wifi->RequestScan(StringVariantMap());
	reply.waitForFinished();
	if(!reply.isError()) return;
	qWarning() << "NetworkManager::requestScan" << reply.error().message();
}

#define NETWORK_MANAGER_GROUP "NetworkManager"
#define ON_KEY "on"

bool NetworkManager::turnOn()
{
	m_nm->Enable(true);
	QSettings settings;
	settings.beginGroup(NETWORK_MANAGER_GROUP);
	settings.setValue(ON_KEY, true);
	settings.endGroup();
	return true; // TODO: This is a bad assumption
}

void NetworkManager::turnOff()
{
	m_nm->Enable(false);
	QSettings settings;
	settings.beginGroup(NETWORK_MANAGER_GROUP);
	settings.setValue(ON_KEY, false);
	settings.endGroup();
}


bool NetworkManager::enableAP()
{
#ifdef WALLABY
	int ret = system("sudo /usr/bin/python /usr/bin/wifi_configurator.py &");
	return (ret == 0);
#endif
	return true;
}


bool NetworkManager::disableAP()
{
#ifdef WALLABY
	int ret = system("sudo ifconfig wlan0 down");
	return (ret == 0);
#endif
	return true;
}


bool NetworkManager::isOn() const
{
	return m_nm->networkingEnabled();
}

bool NetworkManager::isPersistentOn() const
{
	QSettings settings;
	settings.beginGroup(NETWORK_MANAGER_GROUP);
	bool ret = settings.value(ON_KEY, false).toBool();
	settings.endGroup();
	return ret;
}

NetworkManager::State NetworkManager::state() const
{
	return (NetworkManager::State)m_device->state();
}

Network NetworkManager::active() const
{
	if(!m_wifi) return Network();
	return createAccessPoint(m_wifi->activeAccessPoint());
}

NetworkList NetworkManager::accessPoints() const
{
	if(!m_wifi) return NetworkList();
	QList<QDBusObjectPath> aps = m_wifi->GetAccessPoints().value();
	NetworkList networks;
	foreach(const QDBusObjectPath& ap, aps) {
		networks << createAccessPoint(ap);
	}
	
	return networks;
}

QString NetworkManager::ipAddress() const
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifreq ifr;
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ - 1);
	QString ret = "";
	if(!ioctl(fd, SIOCGIFADDR, &ifr)) {
		ret = QString(inet_ntoa(((sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	}
	close(fd);
	return ret;
}

NetworkManager::NetworkManager()
	: m_nm(new NMNetworkManager(NM_SERVICE, NM_OBJECT, QDBusConnection::systemBus(), this)),
	m_device(0),
	m_wifi(0)
{
	// Register our metatype with dbus
	qDBusRegisterMetaType<Connection>();
	qDBusRegisterMetaType<StringVariantMap>();
	
	QDBusPendingReply<QList<QDBusObjectPath> > reply = m_nm->GetDevices();
	
	if(reply.isError()) return;
	
	QList<QDBusObjectPath> objectPaths = reply.value();
	QDBusObjectPath wifiPath;
	foreach(const QDBusObjectPath &objectPath, objectPaths) {
		NMDevice device(NM_SERVICE, objectPath.path(), QDBusConnection::systemBus());
		if(device.deviceType() == NM_DEVICE_TYPE_WIFI) wifiPath = objectPath;
	}
	
	if(wifiPath.path().isEmpty()) {
		qWarning() << "No wifi device found. Disabling access point scanning";
		return;
	}
	
	qDebug() << "Wifi device found.";
	
	m_device = new NMDevice(NM_SERVICE, wifiPath.path(), QDBusConnection::systemBus(), this);
	m_wifi = new NMDeviceWifi(NM_SERVICE, wifiPath.path(), QDBusConnection::systemBus(), this);
	
	connect(m_device, SIGNAL(StateChanged(uint, uint, uint)),
		SLOT(stateChangedBouncer(uint, uint)));
	connect(m_wifi, SIGNAL(AccessPointAdded(QDBusObjectPath)),
		SLOT(nmAccessPointAdded(QDBusObjectPath)));
	connect(m_wifi, SIGNAL(AccessPointRemoved(QDBusObjectPath)),
		SLOT(nmAccessPointRemoved(QDBusObjectPath)));
	
	if(isPersistentOn()) turnOn(); else turnOff();
	
	requestScan();
}

void NetworkManager::nmAccessPointAdded(const QDBusObjectPath &accessPoint)
{
	Network network = createAccessPoint(accessPoint);
	qDebug() << "Access Point Added: " << network;
	
	emit accessPointAdded(network);
	// m_accessPoints.append(network);
}

void NetworkManager::nmAccessPointRemoved(const QDBusObjectPath &accessPoint)
{
	Network network = createAccessPoint(accessPoint);
	qDebug() << "Access Point Removed: " << network;
	
	m_accessPoints.removeAll(network);
	// emit accessPointRemoved(network);
}

void NetworkManager::stateChangedBouncer(uint newState, uint oldState)
{
	NetworkManager::State networkStateNew = (NetworkManager::State)newState;
	NetworkManager::State networkStateOld = (NetworkManager::State)oldState;
	emit stateChanged(networkStateNew, networkStateOld);
}

Network NetworkManager::networkFromConnection(const Connection &connection) const
{
	// TODO: It would be nice to make this static somewhere
	QMap<QString, Network::Security> securityMap;
	securityMap["none"] = Network::None;
	securityMap["wep"] = Network::Wep;
	securityMap["ieee8021x"] = Network::DynamicWep;
	securityMap["wpa-psk"] = Network::Wpa;
	securityMap["wpa-epa"] = Network::WpaEnterprise;
	
	QMap<QString, Network::Mode> modeMap;
	modeMap["infrastructure"] = Network::Infrastructure;
	modeMap["adhoc"] = Network::AdHoc;
	
	Network network;
	network.setSsid(connection["802-11-wireless"]["ssid"].toString());
	network.setMode(modeMap[connection["802-11-wireless"]["mode"].toString()]);
	qDebug() << "Connection" << connection["802-11-wireless"]["ssid"].toString()
		<< connection["802-11-wireless-security"]["security"].toString();
	network.setSecurity(securityMap[connection["802-11-wireless-security"]["security"].toString()]);
	// Technically, password only applies to WEP connections. We always store both password
	// and psk, however, so it is a somewhat safe assumption to only try the password
	// entry.
	network.setPassword(connection["802-11-wireless"]["password"].toString());
	return network;
}

Network NetworkManager::createAccessPoint(const QDBusObjectPath &accessPoint) const
{
	NMAccessPoint accessPointObject(NM_SERVICE, accessPoint.path(), QDBusConnection::systemBus());
	
	Network newNetwork;
	// SSID
	newNetwork.setSsid(accessPointObject.ssid());
	newNetwork.setStrength(accessPointObject.strength());
	
	// Security
	const uint securityMode = accessPointObject.wpaFlags();
	
	// FIXME: How do we detect Wpa Enterprise?
	// What about Dynamic WEP?
	switch(securityMode) {
	case NM_802_11_AP_SEC_NONE:
		newNetwork.setSecurity(Network::None);
		break;
	case NM_802_11_AP_SEC_PAIR_WEP40:
	case NM_802_11_AP_SEC_PAIR_WEP104:
	case NM_802_11_AP_SEC_GROUP_WEP40:
	case NM_802_11_AP_SEC_GROUP_WEP104:
		newNetwork.setSecurity(Network::Wep);
		break;
	default:
		newNetwork.setSecurity(Network::Wpa);
		break;
	}
	
	// Mode
	const uint mode = accessPointObject.mode();
	switch(mode) {
	case NM_802_11_MODE_ADHOC:
		newNetwork.setMode(Network::AdHoc);
		break;
	case NM_802_11_MODE_INFRA:
		newNetwork.setMode(Network::Infrastructure);
		break;
	default:
		newNetwork.setMode(Network::Unknown);
		break;
	}
	
	return newNetwork;
}

#endif
