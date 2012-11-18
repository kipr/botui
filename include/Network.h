#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <QString>
#include <QList>

class Network
{
public:
	enum Security {
		None = 0,
		Wep,
		DynamicWep,
		Wpa,
		WpaEnterprise
	};
	
	Network();
	
	void setSsid(const QString& ssid);
	const QString& ssid() const;
	
	void setSecurity(const Security& security);
	const Security& security() const;
	
	void setPassword(const QString& password);
	const QString& password() const;
private:
	QString m_ssid;
	Security m_security;
	QString m_password;
};

typedef QList<Network> NetworkList;

#endif
