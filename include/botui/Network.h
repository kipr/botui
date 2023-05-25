#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <QList>
#include <QString>

class Network {
  public:
    enum Security { None = 0, Wep, DynamicWep, Wpa, WpaEnterprise };

    enum Mode { Unknown = 0, AdHoc, Infrastructure, AP };

    Network();

    void setAPPath(const QString &path);
    const QString &apPath() const;

    void setSsid(const QString &ssid);
    const QString &ssid() const;

    void setSecurity(const Security &security);
    const Security &security() const;
    QString securityString() const;

    void setPassword(const QString &password);
    const QString &password() const;

    void setMode(const Mode &mode);
    const Mode &mode() const;

    void setStrength(const double &strength);
    const double &strength() const;

    bool isComplete() const;

  private:
    QString m_ssid;
    Security m_security;
    QString m_password;
    Mode m_mode;
    double m_strength;
    QString m_apPath;
};

typedef QList<Network> NetworkList;

bool operator==(const Network &lhs, const Network &rhs);

QDebug operator<<(QDebug dbg, const Network &network);

#endif
