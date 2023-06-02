#include "Options.h"

#include "NetworkManager.h"
#include "SystemUtils.h"

#include "org_freedesktop_NetworkManager.h"
#include "org_freedesktop_NetworkManager_AccessPoint.h"
#include "org_freedesktop_NetworkManager_Device.h"
#include "org_freedesktop_NetworkManager_Device_Wireless.h"
#include "org_freedesktop_NetworkManager_Settings.h"
#include "org_freedesktop_NetworkManager_Settings_Connection.h"
#include "org_freedesktop_NetworkManager_Connection_Active.h"
#include "org_freedesktop_NetworkManager_IP4Config.h"

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
// AP Mode
#define NM_802_11_MODE_AP 3
#define NM_SERVICE "org.freedesktop.NetworkManager"
#define NM_OBJECT "/org/freedesktop/NetworkManager"
QDBusObjectPath AP_PATH;
Connection DEFAULT_AP;

#define WIFI_DEVICE "wlan0" // always wlan0 for raspberry pi
#define AP_NAME m_dev->serial() + "-wombatAP"
#define AP_SSID (AP_NAME).toUtf8()
#define AP_PASSWORD SystemUtils::sha256(m_dev->id()).left(6) + "00"

NetworkManager::~NetworkManager()
{
}

#define NM_802_11_WIRELESS_KEY ("802-11-wireless")
#define NM_802_11_SECURITY_KEY ("802-11-wireless-security")

void NetworkManager::addNetwork(const Network &network)
{
  // Yes, yes... this is a hard coded mess.
  // Maybe this should be generalized in the future.
  qDebug() << "Network Mode: " << network.mode();
  if (network.mode() == Network::Infrastructure)
  {
    Connection connection;
    connection["ipv4"]["method"] = "auto";
    connection["ipv6"]["method"] = "auto";

    // For now we only handle wifi
    connection["connection"]["type"] = "802-11-wireless";
    connection["connection"]["uuid"] = QUuid::createUuid().toString().remove('{').remove('}');
    // File name is just the SSID for now
    connection["connection"]["id"] = network.ssid();
    if (network.ssid().toLatin1() != AP_SSID)
    {
      connection["connection"]["autoconnect-priority"] = 100;
    }
    else
    {
      connection["connection"]["autoconnect-priority"] = -900;
    }

    // SSID
    connection[NM_802_11_WIRELESS_KEY]["ssid"] = network.ssid().toLatin1();

    // Network Mode (adhoc or infrastructure)
    switch (network.mode())
    {
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
        "none",
        "wep",
        "ieee8021x",
        "wpa-psk",
        "wpa-epa"};

    if (network.security() != Network::None)
    {

      qDebug() << "addNetwork password: " << network.password();
      connection[NM_802_11_SECURITY_KEY]["key-mgmt"] = securityTypes[network.security()];
      // WEP uses this key
      connection[NM_802_11_SECURITY_KEY]["password"] = network.password();
      // WPA uses this one
      connection[NM_802_11_SECURITY_KEY]["psk"] = network.password();

      // Finally, tell our configuration about the security
      connection[NM_802_11_WIRELESS_KEY]["security"] = NM_802_11_SECURITY_KEY;
    }

    // Send our config via dbus to NetworkManager
    OrgFreedesktopNetworkManagerSettingsInterface settings(
        NM_SERVICE,
        NM_OBJECT "/Settings",
        QDBusConnection::systemBus());

    QDBusObjectPath selectedNet;
    bool foundNetwork = false;
    QList<QDBusObjectPath> listedConnections = settings.ListConnections(); // All settings connections known

    foreach (const QDBusObjectPath &setConPath, listedConnections)
    {
      OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
          NM_SERVICE,
          setConPath.path(),
          QDBusConnection::systemBus());

      Connection details = conn.GetSettings().value();
      if (details["connection"]["id"] == network.ssid())
      {
        selectedNet = setConPath;
        foundNetwork = true;
      }
    }

    if (foundNetwork)
    {
      m_nm->ActivateConnection(selectedNet, devicePath, QDBusObjectPath("/"));
    }
    else
    {
      m_nm->AddAndActivateConnection(connection, devicePath, QDBusObjectPath(network.apPath()));
    }
  }

  emit networkAdded(network);
}

void NetworkManager::forgetNetwork(const Network &network)
{
  OrgFreedesktopNetworkManagerSettingsInterface settings(
      NM_SERVICE,
      NM_OBJECT "/Settings",
      QDBusConnection::systemBus());
  QList<QDBusObjectPath> connections = settings.ListConnections();

  Q_FOREACH (const QDBusObjectPath &connectionPath, connections)
  {
    OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
        NM_SERVICE,
        connectionPath.path(),
        QDBusConnection::systemBus());

    Connection details = conn.GetSettings().value();

    // This connection is not a wifi one. Skip.
    if (!details.contains("802-11-wireless"))
      continue;

    if (network.ssid() == details["802-11-wireless"]["ssid"].toString())
    {
      conn.Delete();
    }
  }

  emit networkForgotten(network);
}

QString NetworkManager::getAPName()
{
  return AP_NAME;
}

NetworkList NetworkManager::networks() const
{
  NetworkList networks;
  OrgFreedesktopNetworkManagerSettingsInterface settings(
      NM_SERVICE,
      NM_OBJECT "/Settings",
      QDBusConnection::systemBus());
  QList<QDBusObjectPath> connections = settings.ListConnections();

  Q_FOREACH (const QDBusObjectPath &connectionPath, connections)
  {
    OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
        NM_SERVICE,
        connectionPath.path(),
        QDBusConnection::systemBus());

    Connection details = conn.GetSettings().value();

    // This connection is not a wifi one. Skip.
    if (!details.contains("802-11-wireless"))
      continue;

    networks << networkFromConnection(details);
  }

  return networks;
}

void NetworkManager::requestScan()
{
  if (!m_wifi)
    return;
  qDebug() << "Requesting scan";
  QDBusPendingReply<> reply = m_wifi->RequestScan(StringVariantMap());
  reply.waitForFinished();
  if (!reply.isError())
    return;
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

  QDBusObjectPath apPath = getAPSettingsObjectPath();

  if (apPath.path() != "") // AP Configuration already exists
  {
    qDebug() << "AP Path Connection already exists";
    if (NetworkManager::ref().isActiveConnectionOn() == true)
    {
      m_nm->DeactivateConnection(m_device->activeConnection()); // Deactivate current connection
    }

    m_nm->ActivateConnection(apPath, devicePath, QDBusObjectPath("/"));
  }
  else
  { // AP Configuration doesn't exist yet
    createAPConfig();
    sleep(3);
    apPath = getAPSettingsObjectPath();
    m_nm->ActivateConnection(apPath, devicePath, QDBusObjectPath("/"));
  }

  return true;
}

QDBusObjectPath NetworkManager::getAPSettingsObjectPath() const
{
  OrgFreedesktopNetworkManagerSettingsInterface settings(
      NM_SERVICE,
      NM_OBJECT "/Settings",
      QDBusConnection::systemBus());

  QList<QDBusObjectPath> listedConnections = settings.ListConnections(); // All settings connections known
  QDBusObjectPath settingsPath;
  foreach (const QDBusObjectPath &setConPath, listedConnections)
  {
    OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
        NM_SERVICE,
        setConPath.path(),
        QDBusConnection::systemBus());

    Connection details = conn.GetSettings().value();
    if (details["connection"]["id"].value<QString>() == AP_NAME)
    {
      settingsPath = setConPath;
      break;
    }
  }

  return settingsPath;
}

bool NetworkManager::disableAP()
{

  OrgFreedesktopNetworkManagerSettingsInterface settings(
      NM_SERVICE,
      NM_OBJECT "/Settings",
      QDBusConnection::systemBus());

  QDBusObjectPath selectedNet;
  QList<QDBusObjectPath> listedConnections = settings.ListConnections(); // All settings connections known

  foreach (const QDBusObjectPath &settingPath, listedConnections)
  {

    OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
        NM_SERVICE,
        settingPath.path(),
        QDBusConnection::systemBus());

    Connection details = conn.GetSettings().value();

    if (details["connection"]["id"] == AP_NAME)
    {
      conn.Delete();
    }
  }
  return true;
}

Connection NetworkManager::createAPConfig() const // Creates a default AP_SSID configuration for settings
{
  qDebug() << "Creating AP Config...";
  Connection apConfig;
  DEFAULT_AP["ipv4"]["method"] = "shared";
  DEFAULT_AP["ipv6"]["method"] = "auto";
  DEFAULT_AP["connection"]["type"] = "802-11-wireless";
  DEFAULT_AP["connection"]["uuid"] = QUuid::createUuid().toString().remove('{').remove('}');
  // File name is just the SSID for now
  DEFAULT_AP["connection"]["id"] = AP_NAME;
  DEFAULT_AP["connection"]["autoconnect"] = false;
  DEFAULT_AP["connection"]["autoconnect-priority"] = -900;
  DEFAULT_AP["connection"]["interface-name"] = WIFI_DEVICE;
  // SSID
  DEFAULT_AP[NM_802_11_WIRELESS_KEY]["ssid"] = AP_SSID;
  DEFAULT_AP[NM_802_11_WIRELESS_KEY]["mode"] = "ap";

  DEFAULT_AP[NM_802_11_WIRELESS_KEY]["security"] = NM_802_11_SECURITY_KEY;

  DEFAULT_AP[NM_802_11_SECURITY_KEY]["key-mgmt"] = "wpa-psk";
  // DEFAULT_AP[NM_802_11_SECURITY_KEY]["pairwise"] = qSList;
  // DEFAULT_AP[NM_802_11_SECURITY_KEY]["proto"] = qProtoList;

  DEFAULT_AP[NM_802_11_SECURITY_KEY]["psk"] = "kipr4609";
  OrgFreedesktopNetworkManagerSettingsInterface settings(
      NM_SERVICE,
      NM_OBJECT "/Settings",
      QDBusConnection::systemBus());
  settings.AddConnection(DEFAULT_AP);
  return DEFAULT_AP;
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
  if (!m_wifi)
    return Network();
  return createAccessPoint(m_wifi->activeAccessPoint());
}

QString NetworkManager::activeConnectionPassword() const
{
  if (!isActiveConnectionOn())
  {
    return "";
  }
  return getPassword(active().ssid());
}

bool NetworkManager::isActiveConnectionOn() const
{
  bool activeConnOn;
  if (m_device->activeConnection().path() != "/") // if there is an Active Connection path (i.e. not "/")
  {
    activeConnOn = true;
  }
  return activeConnOn;
}

QString NetworkManager::currentActiveConnectionName() const
{
  QDBusObjectPath activePath = m_device->activeConnection(); // Device's current active connection

  OrgFreedesktopNetworkManagerConnectionActiveInterface activeConnObj(
      NM_SERVICE,
      activePath.path(),
      QDBusConnection::systemBus());

  return activeConnObj.id();
}

NetworkList NetworkManager::accessPoints() const
{
  if (!m_wifi)
    return NetworkList();
  QList<QDBusObjectPath> aps = m_wifi->GetAllAccessPoints();
  NetworkList networks;
  QList<Network> netList;
  QList<QByteArray> netSSID;
  foreach (const QDBusObjectPath &ap, aps)
  {
    Network newNet = createAccessPoint(ap);
    netList << newNet;
  }
  foreach (const Network &nw, netList)
  {

    if ((netSSID.contains(nw.ssid().toLatin1())))
    {
      // qDebug() << nw.ssid().toLatin1() << " already contained";
      continue;
    }
    else
    {
      netSSID.append(nw.ssid().toLatin1());
      networks << nw;
    }
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
  if (!ioctl(fd, SIOCGIFADDR, &ifr))
  {
    ret = QString(inet_ntoa(((sockaddr_in *)&ifr.ifr_addr)->sin_addr));
  }
  close(fd);
  return ret;
}

QString NetworkManager::ip4Address() const
{
  QString ipAddr;
  QDBusObjectPath ip4Con = m_device->ip4Config(); // ip4Address path
  OrgFreedesktopNetworkManagerIP4ConfigInterface ip4Object(
      NM_SERVICE,
      ip4Con.path(),
      QDBusConnection::systemBus());

  QList<QMap<QString, QVariant>> ip4conf = ip4Object.addressData();
  ipAddr = ip4conf.value(0).value("address").toString();
  return ipAddr;
}

void NetworkManager::init(const Device *device)
{
  m_dev = device;
}

NetworkManager::NetworkManager()
    : m_nm(new OrgFreedesktopNetworkManagerInterface(
          NM_SERVICE,
          NM_OBJECT,
          QDBusConnection::systemBus(),
          this)),
      m_device(0), m_wifi(0), m_dev(nullptr)
{

  // Register our metatype with dbus
  qDBusRegisterMetaType<Connection>();
  qDBusRegisterMetaType<StringVariantMap>();
  qDBusRegisterMetaType<ListStringVariantMap>();
  OrgFreedesktopNetworkManagerSettingsInterface settings(
      NM_SERVICE,
      NM_OBJECT "/Settings",
      QDBusConnection::systemBus());

  QDBusPendingReply<QList<QDBusObjectPath>> reply = m_nm->GetDevices();

  if (reply.isError())
    return;

  QList<QDBusObjectPath> objectPaths = reply.value();
  QDBusObjectPath wifiPath;
  foreach (const QDBusObjectPath &objectPath, objectPaths)
  {
    OrgFreedesktopNetworkManagerDeviceInterface device(
        NM_SERVICE,
        objectPath.path(),
        QDBusConnection::systemBus());
    if (device.deviceType() == NM_DEVICE_TYPE_WIFI)
      wifiPath = objectPath;
  }

  if (wifiPath.path().isEmpty())
  {
    qWarning() << "No wifi device found. Disabling access point scanning";
    return;
  }

  qDebug() << "Wifi device found.";
  qDebug() << wifiPath.path();

  m_device = new OrgFreedesktopNetworkManagerDeviceInterface(
      NM_SERVICE,
      wifiPath.path(),
      QDBusConnection::systemBus(),
      this);
  m_wifi = new OrgFreedesktopNetworkManagerDeviceWirelessInterface(
      NM_SERVICE,
      wifiPath.path(),
      QDBusConnection::systemBus(),
      this);
  devicePath = QDBusObjectPath(m_device->path());
  qDebug() << "Device Path:" << devicePath.path();
  qDebug() << "M_Wifi: " << m_wifi;

  connect(m_device, SIGNAL(StateChanged(uint, uint, uint)),
          SLOT(stateChangedBouncer(uint, uint)));
  connect(m_wifi, SIGNAL(AccessPointAdded(QDBusObjectPath)),
          SLOT(nmAccessPointAdded(QDBusObjectPath)));
  connect(m_wifi, SIGNAL(AccessPointRemoved(QDBusObjectPath)),
          SLOT(nmAccessPointRemoved(QDBusObjectPath)));

  turnOn();

  requestScan();
}

void NetworkManager::nmAccessPointAdded(const QDBusObjectPath &accessPoint)
{
  Network network = createAccessPoint(accessPoint);
  qDebug() << "Access Point Added: " << network << "with path:" << network.apPath();

  emit accessPointAdded(network);
}

void NetworkManager::nmAccessPointRemoved(const QDBusObjectPath &accessPoint)
{
  Network network = createAccessPoint(accessPoint);
  qDebug() << "Access Point Removed: " << network;

  m_accessPoints.removeAll(network);
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
  modeMap["ap"] = Network::AP;

  Network network;
  network.setSsid(connection["802-11-wireless"]["ssid"].toString());
  network.setMode(modeMap[connection["802-11-wireless"]["mode"].toString()]);
  qDebug() << "Connection" << connection["802-11-wireless"]["ssid"].toString()
           << connection[NM_802_11_SECURITY_KEY]["security"].toString();
  network.setSecurity(securityMap[connection[NM_802_11_SECURITY_KEY]["security"].toString()]);
  // Technically, password only applies to WEP connections. We always store both password
  // and psk, however, so it is a somewhat safe assumption to only try the password
  // entry.
  network.setPassword(getPassword(network.ssid()));
  return network;
}

Network NetworkManager::createAccessPoint(const QDBusObjectPath &accessPoint) const
{
  OrgFreedesktopNetworkManagerAccessPointInterface accessPointObject(
      NM_SERVICE,
      accessPoint.path(),
      QDBusConnection::systemBus());

  Network newNetwork;
  // SSID
  newNetwork.setSsid(accessPointObject.ssid());
  newNetwork.setStrength(accessPointObject.strength());
  newNetwork.setAPPath(accessPoint.path());

  // Security
  const uint securityMode = accessPointObject.wpaFlags();

  // FIXME: How do we detect Wpa Enterprise?
  // What about Dynamic WEP?
  switch (securityMode)
  {
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
  switch (mode)
  {
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

  // set password, if available. Note that this works even if the security is None because
  // then the value returned will just be ""
  // newNetwork.setPassword(getPassword(newNetwork.ssid()));

  return newNetwork;
}

QList<QDBusObjectPath> NetworkManager::getAllConnectionPaths() const
{
  // initialize settings provider to get all dbus path connections
  OrgFreedesktopNetworkManagerSettingsInterface settings(
      NM_SERVICE, NM_OBJECT "/Settings", QDBusConnection::systemBus());
  return settings.ListConnections();
}

QList<QPair<Connection, QDBusObjectPath>>
NetworkManager::getAllConnections() const
{
  QList<QPair<Connection, QDBusObjectPath>> ret;

  // iterate through all the paths and get the connections associated with
  // them
  foreach (const QDBusObjectPath &connectionPath, getAllConnectionPaths())
  {
    // get the connection associated with the dbus path connection
    OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
        NM_SERVICE, connectionPath.path(), QDBusConnection::systemBus());
    Connection details = conn.GetSettings().value();

    // This connection is not a wifi one. Skip.
    if (!details.contains(NM_802_11_WIRELESS_KEY))
      continue;

    ret << qMakePair(details, connectionPath);
  }
  return ret;
}

QPair<Connection, QDBusObjectPath>
NetworkManager::getConnection(QString ssid) const
{
  // loop through all the found dbus path connections
  using ConnectionPathPair = QPair<Connection, QDBusObjectPath>;
  foreach (const ConnectionPathPair &pair, getAllConnections())
  {
    // if ssid matches, return the connection and the path
    if (pair.first[NM_802_11_WIRELESS_KEY]["ssid"].toString() == ssid)
    {
      return pair;
    }
  }

  // unable to find the connection and path, so return empty ones
  return qMakePair(Connection(), QDBusObjectPath());
}

QString NetworkManager::getPassword(QString ssid) const
{
  // get connection and path
  QPair<Connection, QDBusObjectPath> pair = getConnection(ssid);

  // if it was unable to find a matching connection, return an empty password
  if (pair.first.isEmpty() || pair.first[NM_802_11_WIRELESS_KEY]["security"] != NM_802_11_SECURITY_KEY)
  {
    return "";
  }
  else
  {
    // get the secrets
    OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(NM_SERVICE, pair.second.path(), QDBusConnection::systemBus());

    QDBusPendingReply<Connection> reply = conn.GetSecrets(NM_802_11_SECURITY_KEY);
    Connection conSecrets = getReply(reply, "getting password");

    // WEP family
    if (pair.first[NM_802_11_SECURITY_KEY]["key-mgmt"].toString() == "wep" || pair.first[NM_802_11_SECURITY_KEY]["key-mgmt"].toString() == "ieee8021x")
      return conSecrets[NM_802_11_SECURITY_KEY]["password"].toString();
    // WPA family
    else if (pair.first[NM_802_11_SECURITY_KEY]["key-mgmt"].toString() == "wpa-psk" || pair.first[NM_802_11_SECURITY_KEY]["key-mgmt"].toString() == "wpa-epa")
      return conSecrets[NM_802_11_SECURITY_KEY]["psk"].toString();
    return "";
  }
}

void NetworkManager::getReply(QDBusPendingReply<> &reply, const QString where, const bool throwError) const
{
  reply.waitForFinished();
  if (reply.isError())
  {
    qDebug() << "ERROR in " << where << ":" << reply.error();
    if (throwError)
    {
      throw reply.error();
    }
  }
}