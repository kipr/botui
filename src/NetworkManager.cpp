#include "Options.h"

#include "NetworkManager.h"
#include "Device.h"

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

#ifndef WOMBAT
#define WIFI_DEVICE "wlp3s0" // varies per pc
#else
#define WIFI_DEVICE "wlan0" // always wlan0 for raspberry pi
#endif
#define AP_NAME m_dev->serial() + "-wombat"
#define AP_SSID (m_dev->serial() + "-wombat").toUtf8()
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
    connection["connection"]["type"] = NM_802_11_WIRELESS_KEY;
    connection["connection"]["uuid"] = QUuid::createUuid().toString().remove('{').remove('}');
    // File name is just the SSID for now
    connection["connection"]["id"] = network.ssid();

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
    qDebug() << "Given Network: " << network << "with AP path: " << network.apPath();
    qDebug() << "Given Network: " << network << "with config path: " << connection;
    Network foundNetwork;

    // Iterate through known Access Points to find desired network to add
    foreach (const Network &nw, accessPoints())
    {
      if (nw.ssid() == network.ssid())
      {
        // qDebug() << "AP Path: " << nw.apPath();
        foundNetwork = nw;
        qDebug() << "Found network: " << foundNetwork << "with path: " << foundNetwork.apPath();
        m_nm->AddAndActivateConnection(connection, devicePath, QDBusObjectPath(foundNetwork.apPath()));

        break;
      }
    }
  }

  if (network.mode() == Network::AP)
  {
    // Send our config via dbus to NetworkManager
    OrgFreedesktopNetworkManagerSettingsInterface settings(
        NM_SERVICE,
        NM_OBJECT "/Settings",
        QDBusConnection::systemBus());

    bool APExist = false;
    QString APuuid;
    QDBusObjectPath apPathConn;
    Connection apCon;
    QList<QDBusObjectPath> connections = settings.ListConnections();

    qDebug() << "Settings Connections: ";
    Q_FOREACH (const QDBusObjectPath &connectionPath, connections)
    {
      OrgFreedesktopNetworkManagerSettingsConnectionInterface apSettInt(
          NM_SERVICE,
          connectionPath.path(),
          QDBusConnection::systemBus());

      Connection detail = apSettInt.GetSettings().value();
      qDebug() << connectionPath.path() << " : " << detail["connection"]["id"];
      if (detail["connection"]["id"] == AP_NAME)
      {
        APExist = true;
        apPathConn = connectionPath;
        // APuuid = detail["connection"]["uuid"].toString();
        qDebug() << "AP Path Connection " << AP_NAME << " already exists";
        break;
      }
    }

    // deactivate current connection if there is one,
    // necessary in order to start an AP on the wifi device
    if (m_device->activeConnection().path() != "")
    {
      QDBusObjectPath curCon = m_device->activeConnection();
      m_nm->DeactivateConnection(curCon);
    }

    // start the AP
    if (!APExist)
    { // if AP Config in Settings doesn't exist already
      qDebug() << "AP Config doesn't exist";

      apCon = createAPConfig();
      QDBusPendingReply<QDBusObjectPath> reply = settings.AddConnection(apCon);
      reply.waitForFinished();
      if (reply.isError())
      {
        qDebug() << "ERROR!!!" << reply.error();
      }

      QDBusObjectPath activeAccess = m_wifi->activeAccessPoint(); // active AP connection
      qDebug() << "so now after making it, activeAccess point is " << activeAccess.path();
    }
    else
    { // if AP Config in Settings DOES exist already
      qDebug() << "exists already with connection" << apPathConn.path() << " device path " << devicePath.path() << "and specific object of /";
      QDBusPendingReply<QDBusObjectPath> reply = m_nm->ActivateConnection(apPathConn, devicePath, QDBusObjectPath("/"));
      reply.waitForFinished();
      if (reply.isError())
      {
        qDebug() << "ERROR second:: " << reply.error();
      }
    }

    // return;
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
    if (!details.contains(NM_802_11_WIRELESS_KEY))
      continue;

    if (network.ssid() == details[NM_802_11_WIRELESS_KEY]["ssid"].toString())
    {
      conn.Delete();
    }
  }

  emit networkForgotten(network);
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
    if (!details.contains(NM_802_11_WIRELESS_KEY))
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
  Connection defaultAPConfig = createAPConfig();
  Network APN = networkFromConnection(defaultAPConfig);
  addNetwork(APN);
  return true;
}

bool NetworkManager::disableAP()
{
  QDBusObjectPath curCon = m_device->activeConnection();
  m_nm->DeactivateConnection(curCon);
  return true;
}

Connection NetworkManager::createAPConfig() const
{
  qDebug() << "Creating AP Config...";

  // new way based off of https://unix.stackexchange.com/questions/234552/create-wireless-access-point-and-share-internet-connection-with-nmcli/310699#310699
  // nmcli connection add type wifi ifname WIFI_DEVICE_NAME con-name CONNECTION_NAME autoconnect no ssid HOTSPOT_SSID
  // nmcli connection modify CONNECTION_NAME 802-11-wireless.mode ap 802-11-wireless.band bg ipv4.method shared
  // nmcli connection modify CONNECTION_NAME wifi-sec.key-mgmt wpa-psk wifi-sec.psk PASSWORD
  // nmcli connection up CONNECTION_NAME
  // nmcli connection show CONNECTION_NAME # this gets the details that we try to emulate
  Connection apConfig;
  // connection settings
  apConfig["connection"]["id"] = AP_NAME;
  apConfig["connection"]["uuid"] = QUuid::createUuid().toString().remove('{').remove('}');
  apConfig["connection"]["type"] = NM_802_11_WIRELESS_KEY;
  apConfig["connection"]["interface-name"] = WIFI_DEVICE; // name of wifi device, varies per machine
  apConfig["connection"]["autoconnect"] = true;
  // 802-11-wireless settings
  apConfig[NM_802_11_WIRELESS_KEY]["ssid"] = AP_SSID;
  apConfig[NM_802_11_WIRELESS_KEY]["mode"] = "ap";
  apConfig[NM_802_11_WIRELESS_KEY]["band"] = "bg";
  // 802-11-wireless-security settings
  apConfig[NM_802_11_SECURITY_KEY]["key-mgmt"] = "wpa-psk";
  apConfig[NM_802_11_SECURITY_KEY]["psk"] = AP_PASSWORD;
  // ip settings
  apConfig["ipv4"]["method"] = "shared";
  apConfig["ipv6"]["method"] = "auto";
  return apConfig;
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

    if (details["802-11-wireless"]["ssid"].toString() == active().ssid())
    {
      QDBusPendingReply<Connection> reply = conn.GetSecrets("802-11-wireless-security");
      reply.waitForFinished();
      if (reply.isError()) // might happen for wired connections
      {
        qDebug() << "reply error " << reply.error();
      }
      else
      {
        return reply.value()["802-11-wireless-security"]["psk"].toString();
      }
    }
  }
  return "";
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

bool NetworkManager::isActiveConnectionAP() const
{
  // not using mode because it detects active AP as mode 2
  // instead, checking if ssid is equal
  return isActiveConnectionOn() && active().ssid() == AP_NAME;
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
  strncpy(ifr.ifr_name, WIFI_DEVICE, IFNAMSIZ - 1);
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
  qDebug() << "IP4 Config path: " << ip4Con.path();
  OrgFreedesktopNetworkManagerIP4ConfigInterface ip4Object(
      NM_SERVICE,
      ip4Con.path(),
      QDBusConnection::systemBus());

  QList<QMap<QString, QVariant>> ip4conf = ip4Object.addressData();
  ipAddr = ip4conf.value(0).value("address").toString();
  return ipAddr;
}

void NetworkManager::init(const Device *dev)
{
  m_dev = dev;
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

void NetworkManager::nmAccessPointAdded(const QDBusObjectPath &accessPoint)
{
  Network network = createAccessPoint(accessPoint);
  qDebug() << "Access Point Added: " << network << "with path:" << network.apPath();

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
  network.setMode(modeMap[connection[NM_802_11_WIRELESS_KEY]["mode"].toString()]);
  qDebug() << "Connection" << connection[NM_802_11_WIRELESS_KEY]["ssid"].toString()
           << connection[NM_802_11_SECURITY_KEY]["security"].toString();
  network.setSecurity(securityMap[connection[NM_802_11_SECURITY_KEY]["security"].toString()]);
  // Technically, password only applies to WEP connections. We always store both password
  // and psk, however, so it is a somewhat safe assumption to only try the password
  // entry.
  network.setPassword(connection[NM_802_11_WIRELESS_KEY]["password"].toString());
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

  return newNetwork;
}
