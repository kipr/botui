#include "Options.h"

#include "Device.h"
#include "NetworkManager.h"

#include "org_freedesktop_NetworkManager.h"
#include "org_freedesktop_NetworkManager_AccessPoint.h"
#include "org_freedesktop_NetworkManager_Connection_Active.h"
#include "org_freedesktop_NetworkManager_Device.h"
#include "org_freedesktop_NetworkManager_Device_Wireless.h"
#include "org_freedesktop_NetworkManager_IP4Config.h"
#include "org_freedesktop_NetworkManager_Settings.h"
#include "org_freedesktop_NetworkManager_Settings_Connection.h"

#include <stdio.h>

#include <string.h> /* for strncpy */

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
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
// AP Mode
#define NM_802_11_MODE_AP 3
#define NM_SERVICE "org.freedesktop.NetworkManager"
#define NM_OBJECT "/org/freedesktop/NetworkManager"
QDBusObjectPath AP_PATH;
Connection DEFAULT_AP;

#ifndef WOMBAT
#define WIFI_DEVICE                                                            \
    "wlp2s0" // varies per pc, almost always need to change this on different
             // computers
#define AP_NAME "COOL_NETWORK"
#define AP_SSID QByteArray("COOL_NETWORK")
#define AP_PASSWORD "COOL_PASSWORD"
#else
#define WIFI_DEVICE "wlan0" // always wlan0 for raspberry pi
#define AP_NAME m_dev->serial() + "-wombat"
#define AP_SSID (m_dev->serial() + "-wombat").toUtf8()
#define AP_PASSWORD SystemUtils::sha256(m_dev->id()).left(6) + "00"
#endif

NetworkManager::~NetworkManager() {}

#define NM_802_11_WIRELESS_KEY ("802-11-wireless")
#define NM_802_11_SECURITY_KEY ("802-11-wireless-security")

void NetworkManager::addNetwork(const Network &network) {
    // deactivate current connection (necessary to properly add/activate new
    // connection)
    if (isActiveConnectionOn()) {
        QDBusObjectPath curCon = m_device->activeConnection();
        m_nm->DeactivateConnection(curCon);
    }

    qDebug() << "Network Mode: " << network.mode();
    if (network.mode() ==
        Network::Infrastructure) // create the connection from the network
    {
        Connection connection; // connection that will be activated
        connection["ipv4"]["method"] = "auto";
        connection["ipv6"]["method"] = "auto";

        // For now we only handle wifi
        connection["connection"]["type"] = NM_802_11_WIRELESS_KEY;
        connection["connection"]["uuid"] =
            QUuid::createUuid().toString().remove('{').remove('}');
        // File name is just the SSID for now
        connection["connection"]["id"] = network.ssid();

        // SSID
        connection[NM_802_11_WIRELESS_KEY]["ssid"] = network.ssid().toLatin1();

        // Network Mode (adhoc or infrastructure)
        switch (network.mode()) {
        case Network::Infrastructure:
            connection[NM_802_11_WIRELESS_KEY]["mode"] = "infrastructure";
            break;
        case Network::AdHoc:
            connection[NM_802_11_WIRELESS_KEY]["mode"] = "adhoc";
            break;
        case Network::AP:
            connection[NM_802_11_WIRELESS_KEY]["mode"] = "ap";
            break;
        default:
            break;
        }

        const static QString securityTypes[] = {
            "none", "wep", "ieee8021x", "wpa-psk", "wpa-epa"};

        if (network.security() != Network::None) {
            connection[NM_802_11_SECURITY_KEY]["key-mgmt"] =
                securityTypes[network.security()];
            switch (network.security()) {
            case Network::DynamicWep:
            case Network::Wep:
                // WEP uses this key
                connection[NM_802_11_SECURITY_KEY]["password"] =
                    network.password();
                break;
            case Network::Wpa:
            case Network::WpaEnterprise:
                // WPA uses this one
                connection[NM_802_11_SECURITY_KEY]["psk"] = network.password();
                break;
            default: // if nothing, then nothing
                break;
            }

            // Finally, tell our configuration about the security
            connection[NM_802_11_WIRELESS_KEY]["security"] =
                NM_802_11_SECURITY_KEY;
        }

        // activate the connection we've created
        activateConnection(connection);
    }

    if (network.mode() == Network::AP) {
        // activate DEFAULT_AP
        AP_PATH = activateConnection(DEFAULT_AP);
    }

    emit networkAdded(network);
}

void NetworkManager::forgetNetwork(const Network &network) {
    using ConnectionPathPair = QPair<Connection, QDBusObjectPath>;
    // loop through all the connections
    foreach (const ConnectionPathPair &pair, getAllConnections()) {
        // if the ssid's match, then delete it
        if (network.ssid() ==
            pair.first[NM_802_11_WIRELESS_KEY]["ssid"].toString()) {
            OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
                NM_SERVICE, pair.second.path(), QDBusConnection::systemBus());
            conn.Delete();
        }
    }

    emit networkForgotten(network);
}

NetworkList NetworkManager::networks() const {
    NetworkList networks;

    // add all the networks
    using ConnectionPathPair = QPair<Connection, QDBusObjectPath>;
    foreach (const ConnectionPathPair &pair, getAllConnections()) {
        networks << networkFromConnection(pair.first);
    }

    return networks;
}

void NetworkManager::requestScan() {
    if (!m_wifi) return;
    qDebug() << "Requesting scan";
    QDBusPendingReply<> reply = m_wifi->RequestScan(StringVariantMap());

    getReply(reply, "NetworkManager::requestScan");
}

#define NETWORK_MANAGER_GROUP "NetworkManager"
#define ON_KEY "on"

bool NetworkManager::turnOn() {
    m_nm->Enable(true);
    QSettings settings;
    settings.beginGroup(NETWORK_MANAGER_GROUP);
    settings.setValue(ON_KEY, true);
    settings.endGroup();
    return true; // TODO: This is a bad assumption
}

void NetworkManager::turnOff() {
    m_nm->Enable(false);
    QSettings settings;
    settings.beginGroup(NETWORK_MANAGER_GROUP);
    settings.setValue(ON_KEY, false);
    settings.endGroup();
}

bool NetworkManager::enableAP() {
    m_nm->DeactivateConnection(m_device->activeConnection()).waitForFinished();
    if (DEFAULT_AP[NM_802_11_WIRELESS_KEY]["mode"] !=
        "ap") // If DEFAULT_AP configuration doesn't exist yet
    {
        qDebug() << "in first thing";
        createAPConfig();
    }
    Network APN = networkFromConnection(DEFAULT_AP);
    addNetwork(APN);
    return true;
}

bool NetworkManager::disableAP() {
    QDBusObjectPath curCon = m_device->activeConnection();
    QDBusPendingReply<> reply = m_nm->DeactivateConnection(curCon);
    getReply(reply, "disabling AP");
    return true;
}

Connection NetworkManager::createAPConfig()
    const // Creates a default APName configuration for settings
{
    qDebug() << "Creating AP Config...";

    // new way based off of
    // https://unix.stackexchange.com/questions/234552/create-wireless-access-point-and-share-internet-connection-with-nmcli/310699#310699
    // nmcli connection add type wifi ifname WIFI_DEVICE_NAME con-name
    // CONNECTION_NAME autoconnect no ssid HOTSPOT_SSID nmcli connection modify
    // CONNECTION_NAME 802-11-wireless.mode ap 802-11-wireless.band bg
    // ipv4.method shared nmcli connection modify CONNECTION_NAME
    // wifi-sec.key-mgmt wpa-psk wifi-sec.psk PASSWORD nmcli connection up
    // CONNECTION_NAME nmcli connection show CONNECTION_NAME # this gets the
    // details that we try to emulate
    DEFAULT_AP["ipv4"]["method"] = "shared";
    DEFAULT_AP["ipv6"]["method"] = "auto";
    DEFAULT_AP["connection"]["type"] = "802-11-wireless";
    DEFAULT_AP["connection"]["uuid"] =
        QUuid::createUuid().toString().remove('{').remove('}');
    // File name is just the SSID for now
    DEFAULT_AP["connection"]["id"] = AP_NAME;
    DEFAULT_AP["connection"]["autoconnect"] = true;
    DEFAULT_AP["connection"]["interface-name"] = WIFI_DEVICE;
    // SSID
    DEFAULT_AP[NM_802_11_WIRELESS_KEY]["ssid"] = AP_SSID;
    DEFAULT_AP[NM_802_11_WIRELESS_KEY]["mode"] = "ap";
    DEFAULT_AP[NM_802_11_WIRELESS_KEY]["band"] = "bg";
    DEFAULT_AP[NM_802_11_WIRELESS_KEY]["security"] = NM_802_11_SECURITY_KEY;

    DEFAULT_AP[NM_802_11_SECURITY_KEY]["key-mgmt"] = "wpa-psk";
    DEFAULT_AP[NM_802_11_SECURITY_KEY]["psk"] = AP_PASSWORD;

    // ip settings
    DEFAULT_AP["ipv4"]["method"] = "shared";
    DEFAULT_AP["ipv6"]["method"] = "auto";

    return DEFAULT_AP;
}

bool NetworkManager::isOn() const { return m_nm->networkingEnabled(); }

bool NetworkManager::isPersistentOn() const {
    QSettings settings;
    settings.beginGroup(NETWORK_MANAGER_GROUP);
    bool ret = settings.value(ON_KEY, false).toBool();
    settings.endGroup();
    return ret;
}

NetworkManager::State NetworkManager::state() const {
    return (NetworkManager::State)m_device->state();
}

Network NetworkManager::active() const {
    if (!m_wifi) return Network();
    return createAccessPoint(m_wifi->activeAccessPoint());
}
QString NetworkManager::activeConnectionPassword() const {
    if (!isActiveConnectionOn()) {
        return "";
    }
    return getPassword(active().ssid());
}

bool NetworkManager::isActiveConnectionOn() const {
    bool activeConnOn;
    if (m_device->activeConnection().path() !=
        "/") // if there is an Active Connection path (i.e. not "/")
    {
        activeConnOn = true;
    }
    return activeConnOn;
}

bool NetworkManager::isActiveConnectionAP() const {
    // not using mode because it detects active AP as mode 2
    // instead, checking if ssid is equal
    return isActiveConnectionOn() && active().ssid() == AP_NAME;
}

NetworkList NetworkManager::accessPoints() const {
    if (!m_wifi) return NetworkList();
    QList<QDBusObjectPath> aps = m_wifi->GetAllAccessPoints();
    NetworkList networks;
    QList<Network> netList;
    QList<QByteArray> netSSID;
    foreach (const QDBusObjectPath &ap, aps) {
        Network newNet = createAccessPoint(ap);
        netList << newNet;
    }
    foreach (const Network &nw, netList) {

        if ((netSSID.contains(nw.ssid().toLatin1()))) {
            // qDebug() << nw.ssid().toLatin1() << " already contained";
            continue;
        }
        else {
            netSSID.append(nw.ssid().toLatin1());
            networks << nw;
        }
    }
    return networks;
}

QString NetworkManager::ipAddress() const {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifreq ifr;
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, WIFI_DEVICE, IFNAMSIZ - 1);
    QString ret = "";
    if (!ioctl(fd, SIOCGIFADDR, &ifr)) {
        ret = QString(inet_ntoa(((sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    }
    close(fd);
    return ret;
}

QString NetworkManager::ip4Address() const {
    QString ipAddr;
    QDBusObjectPath ip4Con = m_device->ip4Config(); // ip4Address path
    OrgFreedesktopNetworkManagerIP4ConfigInterface ip4Object(
        NM_SERVICE, ip4Con.path(), QDBusConnection::systemBus());

    QList<QMap<QString, QVariant>> ip4conf = ip4Object.addressData();
    ipAddr = ip4conf.value(0).value("address").toString();
    return ipAddr;
}

void NetworkManager::init(const Device *dev) { m_dev = dev; }

NetworkManager::NetworkManager()
    : m_nm(new OrgFreedesktopNetworkManagerInterface(
          NM_SERVICE, NM_OBJECT, QDBusConnection::systemBus(), this)),
      m_device(0),
      m_wifi(0),
      m_dev(nullptr) {

    // Register our metatype with dbus
    qDBusRegisterMetaType<Connection>();
    qDBusRegisterMetaType<StringVariantMap>();
    qDBusRegisterMetaType<ListStringVariantMap>();
    OrgFreedesktopNetworkManagerSettingsInterface settings(
        NM_SERVICE, NM_OBJECT "/Settings", QDBusConnection::systemBus());

    QDBusPendingReply<QList<QDBusObjectPath>> reply = m_nm->GetDevices();

    if (reply.isError()) return;

    QList<QDBusObjectPath> objectPaths = reply.value();
    QDBusObjectPath wifiPath;
    foreach (const QDBusObjectPath &objectPath, objectPaths) {
        OrgFreedesktopNetworkManagerDeviceInterface device(
            NM_SERVICE, objectPath.path(), QDBusConnection::systemBus());
        if (device.deviceType() == NM_DEVICE_TYPE_WIFI) wifiPath = objectPath;
    }

    if (wifiPath.path().isEmpty()) {
        qWarning() << "No wifi device found. Disabling access point scanning";
        return;
    }

    qDebug() << "Wifi device found.";
    qDebug() << wifiPath.path();

    m_device = new OrgFreedesktopNetworkManagerDeviceInterface(
        NM_SERVICE, wifiPath.path(), QDBusConnection::systemBus(), this);
    m_wifi = new OrgFreedesktopNetworkManagerDeviceWirelessInterface(
        NM_SERVICE, wifiPath.path(), QDBusConnection::systemBus(), this);
    devicePath = QDBusObjectPath(m_device->path());
    qDebug() << "Device Path:" << devicePath.path();
    qDebug() << "M_Wifi: " << m_wifi;

    connect(m_device,
            SIGNAL(StateChanged(uint, uint, uint)),
            SLOT(stateChangedBouncer(uint, uint)));
    connect(m_wifi,
            SIGNAL(AccessPointAdded(QDBusObjectPath)),
            SLOT(nmAccessPointAdded(QDBusObjectPath)));
    connect(m_wifi,
            SIGNAL(AccessPointRemoved(QDBusObjectPath)),
            SLOT(nmAccessPointRemoved(QDBusObjectPath)));

    // qDebug() << m_device->GetAppliedConnection(0).value();
    // if (isPersistentOn())
    // turnOn();
    // else
    // turnOff();
    turnOn();

    requestScan();

    // foreach (const Network &nw, accessPoints())
    // {
    //   qDebug() << nw;
    // }
}

void NetworkManager::nmAccessPointAdded(const QDBusObjectPath &accessPoint) {
    Network network = createAccessPoint(accessPoint);
    qDebug() << "Access Point Added: " << network
             << "with path:" << network.apPath();

    emit accessPointAdded(network);
    // m_accessPoints.append(network);
}

void NetworkManager::nmAccessPointRemoved(const QDBusObjectPath &accessPoint) {
    Network network = createAccessPoint(accessPoint);
    qDebug() << "Access Point Removed: " << network;

    m_accessPoints.removeAll(network);
    // emit accessPointRemoved(network);
}

void NetworkManager::stateChangedBouncer(uint newState, uint oldState) {
    NetworkManager::State networkStateNew = (NetworkManager::State)newState;
    NetworkManager::State networkStateOld = (NetworkManager::State)oldState;
    emit stateChanged(networkStateNew, networkStateOld);
}

QDBusObjectPath
NetworkManager::activateConnection(const Connection &connection) {
    QDBusObjectPath connectionPath;
    QPair<Connection, QDBusObjectPath> pair =
        getConnection(connection[NM_802_11_WIRELESS_KEY]["ssid"].toString());
    if (!pair.first.isEmpty()) // the connection existed already
    {
        connectionPath = pair.second;
    }
    else {
        // add access point to connections
        OrgFreedesktopNetworkManagerSettingsInterface settings(
            NM_SERVICE, NM_OBJECT "/Settings", QDBusConnection::systemBus());
        QDBusPendingReply<QDBusObjectPath> reply =
            settings.AddConnection(connection);
        connectionPath =
            getReply(reply, "adding connection to system connections");
    }
    m_nm->ActivateConnection(connectionPath, devicePath, connectionPath);
    return connectionPath;
}

Network
NetworkManager::networkFromConnection(const Connection &connection) const {
    // setup variables
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
    modeMap["ap"] = Network::AP;

    // setup the network
    Network network;
    network.setSsid(connection[NM_802_11_WIRELESS_KEY]["ssid"].toString());
    network.setMode(
        modeMap[connection[NM_802_11_WIRELESS_KEY]["mode"].toString()]);
    qDebug() << "Connection"
             << connection[NM_802_11_WIRELESS_KEY]["ssid"].toString()
             << connection[NM_802_11_SECURITY_KEY]["security"].toString();
    network.setSecurity(
        securityMap[connection[NM_802_11_SECURITY_KEY]["security"].toString()]);
    // Technically, password only applies to WEP connections. We always store
    // both password and psk, however, so it is a somewhat safe assumption to
    // only try the password entry.
    network.setPassword(getPassword(network.ssid()));
    return network;
}

Network
NetworkManager::createAccessPoint(const QDBusObjectPath &accessPoint) const {
    OrgFreedesktopNetworkManagerAccessPointInterface accessPointObject(
        NM_SERVICE, accessPoint.path(), QDBusConnection::systemBus());

    Network newNetwork;
    // SSID
    newNetwork.setSsid(accessPointObject.ssid());
    newNetwork.setStrength(accessPointObject.strength());
    newNetwork.setAPPath(accessPoint.path());

    // Security
    // see
    // https://developer-old.gnome.org/NetworkManager/stable/nm-dbus-types.html#NM80211ApFlags
    const uint flags = accessPointObject.flags();
    if (flags <= 4) {
        newNetwork.setSecurity(static_cast<Network::Security>(flags));
    }
    else {
        // fallback is no security
        newNetwork.setSecurity(Network::None);
    }

    // Mode
    const uint mode = accessPointObject.mode();
    switch (mode) {
    case NM_802_11_MODE_ADHOC:
        newNetwork.setMode(Network::AdHoc);
        break;
    case NM_802_11_MODE_INFRA:
        newNetwork.setMode(Network::Infrastructure);
        break;
    case NM_802_11_MODE_AP:
        newNetwork.setMode(Network::AP);
        break;
    default:
        newNetwork.setMode(Network::Unknown);
        break;
    }

    // set password, if available. Note that this works even if the security is
    // None because then the value returned will just be ""
    newNetwork.setPassword(getPassword(newNetwork.ssid()));

    return newNetwork;
}

QList<QDBusObjectPath> NetworkManager::getAllConnectionPaths() const {
    // initialize settings provider to get all dbus path connections
    OrgFreedesktopNetworkManagerSettingsInterface settings(
        NM_SERVICE, NM_OBJECT "/Settings", QDBusConnection::systemBus());
    return settings.ListConnections();
}

QList<QPair<Connection, QDBusObjectPath>>
NetworkManager::getAllConnections() const {
    QList<QPair<Connection, QDBusObjectPath>> ret;

    // iterate through all the paths and get the connections associated with
    // them
    foreach (const QDBusObjectPath &connectionPath, getAllConnectionPaths()) {
        // get the connection associated with the dbus path connection
        OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
            NM_SERVICE, connectionPath.path(), QDBusConnection::systemBus());
        Connection details = conn.GetSettings().value();

        // This connection is not a wifi one. Skip.
        if (!details.contains(NM_802_11_WIRELESS_KEY)) continue;

        ret << qMakePair(details, connectionPath);
    }
    return ret;
}

QPair<Connection, QDBusObjectPath>
NetworkManager::getConnection(QString ssid) const {
    // loop through all the found dbus path connections
    using ConnectionPathPair = QPair<Connection, QDBusObjectPath>;
    foreach (const ConnectionPathPair &pair, getAllConnections()) {
        // if ssid matches, return the connection and the path
        if (pair.first[NM_802_11_WIRELESS_KEY]["ssid"].toString() == ssid) {
            return pair;
        }
    }

    // unable to find the connection and path, so return empty ones
    return qMakePair(Connection(), QDBusObjectPath());
}

QString NetworkManager::getPassword(QString ssid) const {
    // get connection and path
    QPair<Connection, QDBusObjectPath> pair = getConnection(ssid);

    // if it was unable to find a matching connection, return an empty password
    if (pair.first.isEmpty()) {
        return "";
    }
    else {
        // get the secrets
        OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
            NM_SERVICE, pair.second.path(), QDBusConnection::systemBus());

        QDBusPendingReply<Connection> reply =
            conn.GetSecrets(NM_802_11_SECURITY_KEY);
        return getReply(reply,
                        "getting password")[NM_802_11_SECURITY_KEY]["psk"]
            .toString();
    }
}

void NetworkManager::getReply(QDBusPendingReply<> &reply,
                              const QString where,
                              const bool throwError) const {
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "ERROR in " << where << ":" << reply.error();
        if (throwError) {
            throw reply.error();
        }
    }
}