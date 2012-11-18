#include "Network.h"

void Network::setSsid(const QString& ssid)
{
	m_ssid = ssid;
}

const QString& Network::ssid() const
{
	return m_ssid;
}

void Network::setSecurity(const Network::Security& security)
{
	m_security = security;
}

const Network::Security& Network::security() const
{
	return m_security;
}

void Network::setPassword(const QString& password)
{
	m_password = password;
}

const QString& Network::password() const
{
	return m_password;
}