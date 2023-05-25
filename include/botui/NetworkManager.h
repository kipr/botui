#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_

#include "Device.h"
#include "Options.h"
#include "SystemUtils.h"

#ifdef NETWORK_ENABLED

#include "Network.h"
#include "NetworkManagerConnection.h"
#include "Singleton.h"

#include <QObject>

#include <QDBusObjectPath>
#include <QDBusReply>

class OrgFreedesktopNetworkManagerInterface;
class OrgFreedesktopNetworkManagerDeviceInterface;
class OrgFreedesktopNetworkManagerDeviceWirelessInterface;

class NetworkManager : public QObject, public Singleton<NetworkManager> {
    Q_OBJECT
  public:
    enum State {
        Unknown = 0,
        Unmanaged = 10,
        Unavailable = 20,
        Disconnected = 30,
        Prepare = 40,
        Config = 50,
        NeedAuth = 60,
        IpConfig = 70,
        IpCheck = 80,
        Secondaries = 90,
        Activated = 100,
        Deactivating = 110,
        Failed = 120
    };

    NetworkManager();
    ~NetworkManager();
    void init(const Device *device);

    void addNetwork(const Network &network);
    void forgetNetwork(const Network &network);
    NetworkList networks() const;

    Network active() const;
    QString activeConnectionPassword() const;

    void requestScan();

    bool isOn() const;
    bool isPersistentOn() const;

    State state() const;

    NetworkList accessPoints() const;

    QString ipAddress() const;
    QString ip4Address() const;

    Connection createAPConfig() const;

    bool isActiveConnectionOn() const;
    bool isActiveConnectionAP() const;

  public slots:
    bool turnOn();
    void turnOff();

    bool enableAP();
    bool disableAP();

  signals:
    void networkAdded(const Network &network);
    void networkForgotten(const Network &network);

    void accessPointAdded(const Network &network);
    void accessPointRemoved(const Network &network);

    void stateChanged(const NetworkManager::State &newState,
                      const NetworkManager::State &oldState);

  private slots:
    void nmAccessPointAdded(const QDBusObjectPath &path);
    void nmAccessPointRemoved(const QDBusObjectPath &path);

    void stateChangedBouncer(uint newState, uint oldState);

  private:
    /**
     * @brief Activates the given connection, and adds it if it
     * wasn't already in system connections
     *
     * @return QDBusObjectPath the dbus path corresponding to the connection
     */
    QDBusObjectPath activateConnection(const Connection &connection);

    Network networkFromConnection(const Connection &connection) const;
    Network createAccessPoint(const QDBusObjectPath &accessPoint) const;

    /**
     * @brief Get all the Connection Paths
     *
     * @return QList<QDBusObjectPath>
     */
    QList<QDBusObjectPath> getAllConnectionPaths() const;

    /**
     * @brief Get a list of all the Connections and all their corresponding
     * paths
     *
     * @return QList<QPair<Connection, QDBusObjectPath>> a list containing all
     * the Connection/Path pairs
     */
    QList<QPair<Connection, QDBusObjectPath>> getAllConnections() const;

    /**
     * @brief Get the Connection Path object associated with the connection
     * associated with the ssid
     *
     * @param ssid the ssid of the network
     * @return QPair<Connection, QDBusObjectPath> the path, if available, else
     * an empty path
     */
    QPair<Connection, QDBusObjectPath> getConnection(QString ssid) const;

    /**
     * @brief Get the Password of a specific network
     *
     * @param ssid the ssid of the network
     * @return QString the password, if available, else ""
     */
    QString getPassword(QString ssid) const;

    /**
     * @brief Get the Reply object
     * @throws QDBusError - if the reply errored
     *
     * @param reply - the reply to wait for
     * @param where - if errors, where did it error
     */
    void getReply(QDBusPendingReply<> &reply,
                  const QString where = "getting reply",
                  const bool throwError = false) const;

    /**
     * @brief Get the Reply object
     * @throws QDBusError - if the reply errored
     *
     * @tparam Value - return type
     * @tparam Other - any additional types
     * @param reply the reply to wait for
     * @param where if errors, where did it error
     * @return Value the returned value
     */
    template <typename Value, typename... Other>
    Value getReply(QDBusPendingReply<Value, Other...> &reply,
                   const QString where = "getting reply",
                   const bool throwError = false) const {
        getReply(
            reinterpret_cast<QDBusPendingReply<> &>(reply), where, throwError);
        return reply.value();
    }

    NetworkList m_accessPoints;
    QDBusObjectPath devicePath;

    OrgFreedesktopNetworkManagerInterface *m_nm;
    OrgFreedesktopNetworkManagerDeviceInterface *m_device;
    OrgFreedesktopNetworkManagerDeviceWirelessInterface *m_wifi;

    const Device *m_dev;
};

#endif

#endif
