#include "Network.h"

#include <QDebug>

Network::Network() {}

void Network::setAPPath(const QString &path) { m_apPath = path; }

const QString &Network::apPath() const { return m_apPath; }

void Network::setSsid(const QString &ssid) { m_ssid = ssid; }

const QString &Network::ssid() const { return m_ssid; }

void Network::setSecurity(const Network::Security &security) {
    m_security = security;
}

const Network::Security &Network::security() const { return m_security; }

QString Network::securityString() const {
    switch (m_security) {
    case Network::None:
        return "None";
    case Network::Wep:
        return "WEP";
    case Network::DynamicWep:
        return "Dynamic WEP";
    case Network::Wpa:
        return "WPA";
    case Network::WpaEnterprise:
        return "WPA Enterprise";
    }
    return "Unknown";
}

void Network::setPassword(const QString &password) { m_password = password; }

const QString &Network::password() const { return m_password; }

void Network::setMode(const Network::Mode &mode) { m_mode = mode; }

const Network::Mode &Network::mode() const { return m_mode; }

void Network::setStrength(const double &strength) { m_strength = strength; }

const double &Network::strength() const { return m_strength; }

bool Network::isComplete() const {
    if (m_security == Network::None) return true;
    return !m_password.isEmpty();
}

bool operator==(const Network &lhs, const Network &rhs) {
    return lhs.ssid() == rhs.ssid();
}

QDebug operator<<(QDebug dbg, const Network &network) {
    dbg.nospace() << network.ssid() << "("
                  << (network.mode() == Network::AP ? "ap" : "infrastructure")
                  << ")"
                  << " with ";

    switch (network.security()) {
    case Network::None:
        dbg.nospace() << "no security";
        return dbg.space();
    case Network::Wep:
        dbg.nospace() << "WEP";
        break;
    case Network::DynamicWep:
        dbg.nospace() << "Dynamic WEP";
        break;
    case Network::Wpa:
        dbg.nospace() << "WPA";
        break;
    case Network::WpaEnterprise:
        dbg.nospace() << "WPA Enterprise";
        break;
    }
    dbg.nospace() << " (password: " << network.password() << ")";
    return dbg.space();
}
