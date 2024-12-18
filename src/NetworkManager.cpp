#include "Options.h"

#include "NetworkManager.h"
#include "SystemUtils.h"
#include "StandardWidget.h"
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
QString RASPBERRYPI_TYPE;
#ifdef WOMBAT
#define WIFI_DEVICE "wlan0" // always wlan0 for raspberry pi
#else
#define WIFI_DEVICE "wlo1" // wlo1 for dev machine
#endif


#define AP_NAME m_dev->serial() + "-wombat"
#define AP_SSID (AP_NAME).toUtf8()
#define AP_PASSWORD SystemUtils::sha256(m_dev->id()).left(6) + "00"

#define NETWORK_MANAGER_GROUP "NetworkManager"
#define ON_KEY "on"

#define NM_802_11_WIRELESS_KEY ("802-11-wireless")
#define NM_802_11_SECURITY_KEY ("802-11-wireless-security")

NetworkManager::~NetworkManager()
{
}

OrgFreedesktopNetworkManagerInterface *NetworkManager::networkManagerInterface()
{
  static OrgFreedesktopNetworkManagerInterface nmInterface(
      NM_SERVICE,
      NM_OBJECT,
      QDBusConnection::systemBus());

  return &nmInterface;
}

// Adds a new connection to the NetworkManager
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

    connection[NM_802_11_WIRELESS_KEY]["mode"] = "infrastructure";

    // according to the below link, wep uses none
    // https://people.freedesktop.org/~lkundrak/nm-dbus-api/nm-settings.html
    const static QString securityTypes[] = {
        "none",
        "none",
        "ieee8021x",
        "wpa-psk",
        "wpa-epa"};

    if (network.security() != Network::None)
    {

      qDebug() << "addNetwork password: " << network.password();
      connection[NM_802_11_SECURITY_KEY]["key-mgmt"] = securityTypes[network.security()];
      switch (network.security())
      {
      case Network::Wep:
        connection[NM_802_11_SECURITY_KEY]["wep-key0"] = network.password();
        break;
      case Network::DynamicWep:
        // WEP uses this key
        connection[NM_802_11_SECURITY_KEY]["password"] = network.password();
        break;
      case Network::Wpa:
      case Network::WpaEnterprise:
        // WPA uses this one
        connection[NM_802_11_SECURITY_KEY]["psk"] = network.password();
        break;
      case Network::None:
      default:
        qDebug() << "this shouldn't happen";
      }

      // Finally, tell our configuration about the security
      connection[NM_802_11_WIRELESS_KEY]["security"] = NM_802_11_SECURITY_KEY;
    }

    QPair<Connection, QDBusObjectPath> pair = getConnection(network.ssid());
    if (!pair.first.empty()) // found a valid connection
    {
      QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> reply = m_nm->ActivateConnection(pair.second, devicePath, QDBusObjectPath("/"));
      getReply(reply);
    }
    else // no connection found, have to add it
    {
      QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> reply = m_nm->AddAndActivateConnection(connection, devicePath, QDBusObjectPath(network.apPath()));
      getReply(reply);
    }
  }

  emit networkAdded(network);
}

// Forgets the given network
void NetworkManager::forgetNetwork(const Network &network)
{
  Q_FOREACH (const QDBusObjectPath &connectionPath, getAllConnectionPaths())
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

// Get the name of the AP connection
QString NetworkManager::getAPName()
{
  return AP_NAME;
}

// Returns list of all networks
NetworkList NetworkManager::networks() const
{
  NetworkList networks;
  using ConnectionPathPair = QPair<Connection, QDBusObjectPath>;
  foreach (const ConnectionPathPair pair, getAllConnections())
  {
    networks << networkFromConnection(pair.first);
  }

  return networks;
}

// Scans for available networks
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


// Turns on the network manager
bool NetworkManager::turnOn()
{
  m_nm->Enable(true);
  QSettings settings;
  settings.beginGroup(NETWORK_MANAGER_GROUP);
  settings.setValue(ON_KEY, true);
  settings.endGroup();
  return true; // TODO: This is a bad assumption
}

// Turns off the network manager
void NetworkManager::turnOff()
{
  m_nm->Enable(false);
  QSettings settings;
  settings.beginGroup(NETWORK_MANAGER_GROUP);
  settings.setValue(ON_KEY, false);
  settings.endGroup();
}

// Returns the AP connection configuration
QString NetworkManager::getAPConnectionConfig()
{
  QString command = QString("cat /etc/NetworkManager/system-connections/%1.nmconnection").arg(AP_NAME);

  QProcess *myProcess = new QProcess(this);
  myProcess->start("sudo", QStringList() << "/bin/sh" << "-c" << command);
  myProcess->waitForFinished();
  QByteArray output = myProcess->readAllStandardOutput();

  return QString(output).trimmed();
}

// Changes the WiFi band and channel
void NetworkManager::changeWifiBands(QString band, int channel)
{

  QDBusObjectPath apPath = getAPSettingsObjectPath();

  OrgFreedesktopNetworkManagerSettingsConnectionInterface connection(NM_SERVICE, apPath.path(), QDBusConnection::systemBus());

  Connection connectionSettings = connection.GetSettings().value();

  // Get old AP configuration
  QString oldBand = connectionSettings[NM_802_11_WIRELESS_KEY]["band"].toString();
  int oldChannel = connectionSettings[NM_802_11_WIRELESS_KEY]["channel"].toInt();

  // Change band and channel to desired values
  connectionSettings[NM_802_11_WIRELESS_KEY]["band"] = QVariant(band);
  connectionSettings[NM_802_11_WIRELESS_KEY]["channel"] = QVariant(channel);
  connection.Update(connectionSettings);

  QDBusObjectPath devicePath = QDBusObjectPath(m_device->path());

  QString connectionUUID = connectionSettings["connection"]["uuid"].toString();

  OrgFreedesktopNetworkManagerInterface nm(NM_SERVICE, NM_OBJECT, QDBusConnection::systemBus());
  QList<QDBusObjectPath> activeConnections = nm.activeConnections();

  QDBusObjectPath correctConnectionPath;

  // Loop through active connection paths to find the correct one
  foreach (const QDBusObjectPath &activeConnectionPath, activeConnections)
  {
    OrgFreedesktopNetworkManagerConnectionActiveInterface activeConn(NM_SERVICE, activeConnectionPath.path(), QDBusConnection::systemBus());
    qDebug() << "Active connection UUID:" << activeConn.uuid();
    if (activeConn.uuid() == connectionUUID)
    {
      correctConnectionPath = activeConnectionPath;
      break;
    }
  }
  qDebug() << "Correct connection ssid " << connectionSettings[NM_802_11_WIRELESS_KEY]["ssid"].toString();
  QPair<Connection, QDBusObjectPath> correctConnectionPair = getConnection(connectionSettings[NM_802_11_WIRELESS_KEY]["ssid"].toString());

  // Activate found connection 
  QDBusPendingReply<QDBusObjectPath> reply = m_nm->ActivateConnection(correctConnectionPair.second, devicePath, QDBusObjectPath("/"));
  reply.waitForFinished();
  getReply(reply);

  if (reply.isError())
  {
    qWarning() << "Error in ActivateConnection:" << reply.error().message();
    return;
  }

  QDBusObjectPath result = reply.value();
  qDebug() << "Connection activated successfully:" << result.path();

  QTimer::singleShot(3000, this, [this, oldBand, oldChannel, apPath]() mutable
                     {

  OrgFreedesktopNetworkManagerSettingsConnectionInterface connection(NM_SERVICE, apPath.path(), QDBusConnection::systemBus());

    auto connectionSettings = connection.GetSettings().value();
    if (connectionSettings.isEmpty()) { // Check if the QMap is empty
        qDebug() << "Failed to get settings or settings are empty.";
        return;
    }

    QString newBand = connectionSettings[NM_802_11_WIRELESS_KEY]["band"].toString();
    int newChannel = connectionSettings[NM_802_11_WIRELESS_KEY]["channel"].toInt();

      emit stateChangedBandBouncer(oldBand, newBand, oldChannel, newChannel); });
}

// Enables AP connection
bool NetworkManager::enableAP()
{

  QDBusPendingReply<bool> nmRunningReply = m_nm->state();

  if (nmRunningReply.isError())
  {
    qDebug() << "NetworkManager is not running. Cannot enable AP.";
    return false; // Ensure NetworkManager is running
  }

  QDBusObjectPath apPath = getAPSettingsObjectPath();

  if (apPath.path() != "") // AP Configuration already exists
  {
    qDebug() << "AP Path: " << apPath.path();
    qDebug() << "AP Path Connection already exists";
    qDebug() << "AP Strength: " << active().strength();
    OrgFreedesktopNetworkManagerSettingsConnectionInterface connection(NM_SERVICE, apPath.path(), QDBusConnection::systemBus());

    Connection settings = connection.GetSettings().value();
    QMap<QString, QVariant> wirelessSettings = settings.value("802-11-wireless");

    bool containsBand = wirelessSettings.contains("band");
    bool containsChannel = wirelessSettings.contains("channel");

    // Check if the current settings contain "band" and "channel"
    if (!containsBand || !containsChannel)
    {
      qDebug() << "Missing 'band' or 'channel' in AP settings. Recreating AP configuration.";

      bool activeConnectionOn = NetworkManager::ref().isActiveConnectionOn();

      // If there is a current connection active, deactivate it
      if (activeConnectionOn == true)
      {
        qDebug() << "Current active connection: " << m_device->activeConnection().path();
        QDBusPendingReply<void> deactivateReply = m_nm->DeactivateConnection(m_device->activeConnection());
        deactivateReply.waitForFinished();

        if (deactivateReply.isError())
        {
          qWarning() << "Error deactivating connection:" << deactivateReply.error().message();
          return false; // Handle the error appropriately
        }
        else
        {
          qDebug() << "Connection deactivated successfully.";
        }
      }

      // Assign the default band and channel based on the Raspberry Pi type
      if (RASPBERRYPI_TYPE == "3B+")
      {
        settings[NM_802_11_WIRELESS_KEY]["band"] = "a";
        settings[NM_802_11_WIRELESS_KEY]["channel"] = 36;
      }
      else if (RASPBERRYPI_TYPE == "3B")
      {
        settings[NM_802_11_WIRELESS_KEY]["band"] = "bg";
        settings[NM_802_11_WIRELESS_KEY]["channel"] = 1;
      }

      qDebug() << "Modified AP settings: band and channel added.";

      // Update the connection settings
      QDBusPendingReply<void> reply = connection.Update(settings);
      reply.waitForFinished();

      if (reply.isError())
      {
        qWarning() << "Error in Update:" << reply.error().message();
        return false;
      }

      if (reply.isValid())
      {
        qDebug() << "AP settings updated successfully.";
        qDebug() << "Connection after update: " << connection.GetSettings().value();
      }

      // Activate updated connection
      QDBusPendingReply<void> activateReply = m_nm->ActivateConnection(apPath, devicePath, QDBusObjectPath("/"));
      activateReply.waitForFinished();

      if (activateReply.isError())
      {
        qWarning() << "Error activating connection:" << activateReply.error().message();
        return false; // Handle the error appropriately
      }
      else
      {
        qDebug() << "Connection activated successfully.";
      }

      qDebug() << "Device is now active. Proceeding to reapply settings.";

      QDBusPendingReply<void> reapplyReply = m_device->Reapply(settings, 0, 0);
      reapplyReply.waitForFinished();

      if (reapplyReply.isError())
      {
        qWarning() << "Error in Reapply:" << reapplyReply.error().message();
        return false;
      }
      else
      {
        qDebug() << "Reapply successful.";
      }

      return true;
    }

    if (NetworkManager::ref().isActiveConnectionOn() == true)
    {
      m_nm->DeactivateConnection(m_device->activeConnection()); // Deactivate current connection
    }
    else if (NetworkManager::ref().isActiveConnectionOn() == false)
    {

      turnOn();
      uint stateReply;
      while (true)
      {
        stateReply = m_device->state();

        if (stateReply == 30)
        {
          qDebug() << "Reached Disconnected state";
          break;
        }
        sleep(0.5);
      }
    }
    m_nm->ActivateConnection(apPath, devicePath, QDBusObjectPath("/"));

    return true;
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

// Returns AP settings object path
QDBusObjectPath NetworkManager::getAPSettingsObjectPath() const
{
  QDBusObjectPath settingsPath = getConnection(AP_NAME).second;

  return settingsPath;
}

// Turns off the AP connection
void NetworkManager::deactivateAP()
{
  qDebug() << "inside deactivateAP()";
  QDBusObjectPath apPath = getAPSettingsObjectPath();
  QDBusObjectPath activePath = m_device->activeConnection();
  qDebug() << "deactivateAP apPath: " << apPath.path();
  qDebug() << "deactivateAP current activePath: " << activePath.path();
  m_nm->DeactivateConnection(activePath);
}

// Deletes the AP connection
bool NetworkManager::disableAP()
{
  foreach (const QDBusObjectPath &settingPath, getAllConnectionPaths())
  {
    OrgFreedesktopNetworkManagerSettingsConnectionInterface conn(
        NM_SERVICE,
        settingPath.path(),
        QDBusConnection::systemBus());

    Connection details = conn.GetSettings().value();

    if (details["connection"]["id"] == AP_NAME)
    {
      conn.Delete();
      sleep(1);
    }
  }
  return true;
}

// Creates a default AP_SSID configuration for settings
Connection NetworkManager::createAPConfig() const 
{
  qDebug() << "Creating AP Config...";

  DEFAULT_AP["connection"]["type"] = NM_802_11_WIRELESS_KEY;
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

  DEFAULT_AP[NM_802_11_SECURITY_KEY]["psk"] = AP_PASSWORD;

  DEFAULT_AP["ipv4"]["method"] = "shared";
  DEFAULT_AP["ipv6"]["method"] = "auto";

  // Assign the default band and channel based on the Raspberry Pi type
  if (RASPBERRYPI_TYPE == "3B+")
  {
    DEFAULT_AP[NM_802_11_WIRELESS_KEY]["band"] = "a";
    DEFAULT_AP[NM_802_11_WIRELESS_KEY]["channel"] = 36;
  }
  else if (RASPBERRYPI_TYPE == "3B")
  {
    DEFAULT_AP[NM_802_11_WIRELESS_KEY]["band"] = "bg";
    DEFAULT_AP[NM_802_11_WIRELESS_KEY]["channel"] = 1;
  }

  OrgFreedesktopNetworkManagerSettingsInterface settings(
      NM_SERVICE,
      NM_OBJECT "/Settings",
      QDBusConnection::systemBus());
  QDBusObjectPath defaultPath = settings.AddConnection(DEFAULT_AP);
  qDebug() << "settings.AddConnection(DEFAULT_AP): " << defaultPath.path();

  // Hardcode WiFi IP address
  QString str = tr("nmcli connection modify %1 ipv4.address 192.168.125.1/24").arg(AP_NAME);
  QByteArray ba = str.toLocal8Bit();
  const char *csys = ba.data();
  system(csys);

  return DEFAULT_AP;
}

// Returns if the network manager is on
bool NetworkManager::isOn() const
{
  return m_nm->networkingEnabled();
}

// Returns if the network manager is persistent
bool NetworkManager::isPersistentOn() const
{
  QSettings settings;
  settings.beginGroup(NETWORK_MANAGER_GROUP);
  bool ret = settings.value(ON_KEY, false).toBool();
  settings.endGroup();
  return ret;
}

// Returns the current state of the network manager
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

// Returns active connection password
QString NetworkManager::activeConnectionPassword() const
{
  if (!isActiveConnectionOn())
  {
    return "";
  }
  return getPassword(active().ssid());
}

// Returns if the active connection is on
bool NetworkManager::isActiveConnectionOn() const
{
  bool activeConnOn;
  if (m_device->activeConnection().path() != "/") // if there is an Active Connection path (i.e. not "/")
  {
    activeConnOn = true;
  }
  else
  {
    qDebug() << "activeConnection.path(): " << m_device->activeConnection().path();
  }
  return activeConnOn;
}

// Returns the current active connection name
QString NetworkManager::currentActiveConnectionName() const
{
  QDBusObjectPath activePath = m_device->activeConnection(); // Device's current active connection

  OrgFreedesktopNetworkManagerConnectionActiveInterface activeConnObj(
      NM_SERVICE,
      activePath.path(),
      QDBusConnection::systemBus());

  return activeConnObj.id();
}

// Returns NetworkList of all access points
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

// Returns network IP address
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

// Returns if event mode is on or off
bool NetworkManager::eventModeState()
{
  QProcess eventModeProcess;
  QString command = "grep '^EVENT_MODE' /home/kipr/wombat-os/configFiles/wifiConnectionMode.txt | awk '{print $2}'";

  eventModeProcess.start("bash", QStringList() << "-c" << command);
  eventModeProcess.waitForFinished();

  QString output = eventModeProcess.readAllStandardOutput().trimmed();

  if (!output.isEmpty())
  {

    if (output == "true")
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    qDebug() << "Failed to read EVENT_MODE.";
  }
}

// Returns the network's IPv4 address
QString NetworkManager::ip4Address() const
{
  QString ipAddr;
  QDBusObjectPath ip4Con = m_device->ip4Config(); // ip4Address path
  OrgFreedesktopNetworkManagerIP4ConfigInterface ip4Object(
      NM_SERVICE,
      ip4Con.path(),
      QDBusConnection::systemBus());

  QList<QMap<QString, QVariant>> ip4conf = ip4Object.addressData();
  qDebug() << "addressData(): " << ip4conf;
  qDebug() << "addressData().value(0): " << ip4conf.value(0);

  ipAddr = ip4conf.value(0).value("address").toString();
  return ipAddr;
}

// Initilization function
void NetworkManager::init(const Device *device)
{
  qDebug() << "Init";
  m_dev = device;
  bool checkEventState = eventModeState();
  int checkNetworkMode = getBootUpNetworkMode();

  if (!checkEventState && (checkNetworkMode != 1 && checkNetworkMode != 2)) // Event Mode enabled
  {
 
    enableAP();
  }
  else if (checkNetworkMode == 1) // Client Mode
  {

    if(currentActiveConnectionName() == AP_NAME)
    {
      disableAP();
    }
  }
  else if (checkNetworkMode == 2) //Wifi off
  {
    turnOff();
  }
}

NetworkManager::NetworkManager()
    : m_nm(new OrgFreedesktopNetworkManagerInterface(
          NM_SERVICE,
          NM_OBJECT,
          QDBusConnection::systemBus(),
          this)),
      m_device(0), m_wifi(0), m_dev(nullptr)
{

  getRaspberryPiType();
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

  qDebug() << "Active strength: " << active().strength();

}

// Returns the network mode on boot up
int NetworkManager::getBootUpNetworkMode()
{
  QProcess networkModeProcess;
  QString command = "grep '^MODE' /home/kipr/wombat-os/configFiles/wifiConnectionMode.txt | awk '{print $2}'";

  networkModeProcess.start("bash", QStringList() << "-c" << command);
  networkModeProcess.waitForFinished();

  QString output = networkModeProcess.readAllStandardOutput().trimmed();
  qDebug() << "getBootUpNetworkMode: " << output.toInt();
  if (!output.isEmpty())
  {
    switch (output.toInt())
    {
    case 0: // AP Mode
      return 0;
    case 1: // Client Mode
      return 1;
    case 2: // Wifi Off
      return 2;
    default:
      qDebug() << "Unknown MODE.";
    }
  }
  else
  {
    qDebug() << "Failed to read MODE.";
  }
}

// Returns the Raspberry Pi type inside Wombat
void NetworkManager::getRaspberryPiType()
{
  QStringList arguments;
  arguments << "-c" << "cat /proc/cpuinfo | grep Revision | awk '{print $3}'";

  QProcess *myProcess = new QProcess(this);
  myProcess->start("/bin/sh", arguments); // Use /bin/sh or /bin/bash to interpret the command
  myProcess->waitForFinished();
  QByteArray output = myProcess->readAllStandardOutput();

  qDebug() << "Revision code output: " << output;
  if (output.trimmed() == "a020d3" || output.trimmed() == "a020d4")
  {
    RASPBERRYPI_TYPE = "3B+";
  }
  else if (output.trimmed() == "a02082" || output.trimmed() == "a22082" || output.trimmed() == "a32082" || output.trimmed() == "a52082" || output.trimmed() == "a22083")
  {
    RASPBERRYPI_TYPE = "3B";
  }
  else
  {
    RASPBERRYPI_TYPE = "Unknown";
  }

  qDebug() << "RASPBERRYPI_TYPE: " << RASPBERRYPI_TYPE;
}

// NetworkManager AP added signal
void NetworkManager::nmAccessPointAdded(const QDBusObjectPath &accessPoint)
{
  Network network = createAccessPoint(accessPoint);
  qDebug() << "Access Point Added: " << network << "with path:" << network.apPath();

  emit accessPointAdded(network);
}

// NetworkManager AP removed signal
void NetworkManager::nmAccessPointRemoved(const QDBusObjectPath &accessPoint)
{
  Network network = createAccessPoint(accessPoint);
  qDebug() << "Access Point Removed: " << network;

  m_accessPoints.removeAll(network);
}

// Signal for when the network state changes
void NetworkManager::stateChangedBouncer(uint newState, uint oldState)
{
  NetworkManager::State networkStateNew = (NetworkManager::State)newState;
  NetworkManager::State networkStateOld = (NetworkManager::State)oldState;
  emit stateChanged(networkStateNew, networkStateOld);
}

// Returns Network config from connection
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
  network.setSsid(connection[NM_802_11_WIRELESS_KEY]["ssid"].toString());
  network.setMode(modeMap[connection[NM_802_11_WIRELESS_KEY]["mode"].toString()]);
  qDebug() << "Connection" << connection[NM_802_11_WIRELESS_KEY]["ssid"].toString()
           << connection[NM_802_11_SECURITY_KEY]["security"].toString();
  network.setSecurity(securityMap[connection[NM_802_11_SECURITY_KEY]["security"].toString()]);
  // Technically, password only applies to WEP connections. We always store both password
  // and psk, however, so it is a somewhat safe assumption to only try the password
  // entry.
  network.setPassword(getPassword(network.ssid()));
  return network;
}

// Creates a network from an access point
Network NetworkManager::createAccessPoint(const QDBusObjectPath &accessPoint) const
{
  OrgFreedesktopNetworkManagerAccessPointInterface accessPointObject(
      NM_SERVICE,
      accessPoint.path(),
      QDBusConnection::systemBus());

  Network newNetwork;
  // SSID
  newNetwork.setSsid(accessPointObject.ssid());
  newNetwork.setStrength(10);
  newNetwork.setAPPath(accessPoint.path());

  // Security
  const uint securityMode = accessPointObject.flags();
  newNetwork.setSecurity(static_cast<Network::Security>(securityMode));

  // Encrypted, but not necessarily WEP, so double check
  if (securityMode == 1)
  {
    uint rsnFlags = accessPointObject.rsnFlags(); // don't use wpaFlags because it isn't necessarily wpa

    // list of all RSN Flags that mean different types of wpa
    // these are, respectively, the following:
    //  NM_802_11_AP_SEC_KEY_MGMT_PSK = 0x00000100,            // WPA/RSN Pre-Shared Key encryption is supported
    //  NM_802_11_AP_SEC_KEY_MGMT_SAE = 0x00000400,            // WPA/RSN Simultaneous Authentication of Equals is supported
    //  NM_802_11_AP_SEC_KEY_MGMT_OWE = 0x00000800,            // WPA/RSN Opportunistic Wireless Encryption is supported
    //  NM_802_11_AP_SEC_KEY_MGMT_OWE_TM = 0x00001000,         // WPA/RSN Opportunistic Wireless Encryption transition mode is supported. Since: 1.26.
    //  NM_802_11_AP_SEC_KEY_MGMT_EAP_SUITE_B_192 = 0x00002000 // WPA3 Enterprise Suite-B 192 bit mode is supported. Since: 1.30.
    static QList<uint> WPARSNFlags = QList<uint>() << 0x00000100 << 0x00000400 << 0x00000800 << 0x00001000 << 0x00002000;

    // list of all rsn flags.
    // these are, respectively, the following:
    //  NM_802_11_AP_SEC_NONE = 0x00000000,                    // the access point has no special security requirements
    //  NM_802_11_AP_SEC_PAIR_WEP40 = 0x00000001,              // 40/64-bit WEP is supported for pairwise/unicast encryption
    //  NM_802_11_AP_SEC_PAIR_WEP104 = 0x00000002,             // 104/128-bit WEP is supported for pairwise/unicast encryption
    //  NM_802_11_AP_SEC_PAIR_TKIP = 0x00000004,               // TKIP is supported for pairwise/unicast encryption
    //  NM_802_11_AP_SEC_PAIR_CCMP = 0x00000008,               // AES/CCMP is supported for pairwise/unicast encryption
    //  NM_802_11_AP_SEC_GROUP_WEP40 = 0x00000010,             // 40/64-bit WEP is supported for group/broadcast encryption
    //  NM_802_11_AP_SEC_GROUP_WEP104 = 0x00000020,            // 104/128-bit WEP is supported for group/broadcast encryption
    //  NM_802_11_AP_SEC_GROUP_TKIP = 0x00000040,              // TKIP is supported for group/broadcast encryption
    //  NM_802_11_AP_SEC_GROUP_CCMP = 0x00000080,              // AES/CCMP is supported for group/broadcast encryption
    //  NM_802_11_AP_SEC_KEY_MGMT_PSK = 0x00000100,            // WPA/RSN Pre-Shared Key encryption is supported
    //  NM_802_11_AP_SEC_KEY_MGMT_802_1X = 0x00000200,         // 802.1x authentication and key management is supported
    //  NM_802_11_AP_SEC_KEY_MGMT_SAE = 0x00000400,            // WPA/RSN Simultaneous Authentication of Equals is supported
    //  NM_802_11_AP_SEC_KEY_MGMT_OWE = 0x00000800,            // WPA/RSN Opportunistic Wireless Encryption is supported
    //  NM_802_11_AP_SEC_KEY_MGMT_OWE_TM = 0x00001000,         // WPA/RSN Opportunistic Wireless Encryption transition mode is supported. Since: 1.26.
    //  NM_802_11_AP_SEC_KEY_MGMT_EAP_SUITE_B_192 = 0x00002000 // WPA3 Enterprise Suite-B 192 bit mode is supported. Since: 1.30.
    static QList<uint> RSNFlagsValues = QList<uint>() << 0x00000000 << 0x00000001 << 0x00000002 << 0x00000004 << 0x00000008 << 0x00000010 << 0x00000020 << 0x00000040 << 0x00000080 << 0x00000100 << 0x00000200 << 0x00000400 << 0x00000800 << 0x00001000 << 0x00002000;

    // reverse iterate and find out which values make up `rsnFlags`
    for (QList<uint>::const_iterator iter = RSNFlagsValues.constEnd() - 1; iter != RSNFlagsValues.constBegin(); --iter)
    {
      if (rsnFlags > *iter)
      {
        rsnFlags -= *iter;
        if (WPARSNFlags.contains(*iter))
        {
          newNetwork.setSecurity(Network::Wpa);
          break;
        }
      }
    }
  }

  // Mode
  const uint mode = accessPointObject.mode();
  newNetwork.setMode(static_cast<Network::Mode>(mode));

  // set password, if available. Note that this works even if the security is None because
  // then the value returned will just be ""
  newNetwork.setPassword(getPassword(newNetwork.ssid()));

  return newNetwork;
}

// Returns a list of all connections
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

// Returns the connection and path for a given ssid
QPair<Connection, QDBusObjectPath>
NetworkManager::getConnection(QString ssid) const
{

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

// Returns the password for a given ssid
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
    if (pair.first[NM_802_11_SECURITY_KEY]["key-mgmt"].toString() == "none" || pair.first[NM_802_11_SECURITY_KEY]["key-mgmt"].toString() == "ieee8021x")
      return conSecrets[NM_802_11_SECURITY_KEY]["wep-key0"].toString();
    // WPA family
    else if (pair.first[NM_802_11_SECURITY_KEY]["key-mgmt"].toString() == "wpa-psk" || pair.first[NM_802_11_SECURITY_KEY]["key-mgmt"].toString() == "wpa-epa")
      return conSecrets[NM_802_11_SECURITY_KEY]["psk"].toString();
    return "";
  }
}

// Returns the reply from a given query
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