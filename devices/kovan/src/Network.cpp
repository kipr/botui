#include "Network.h"
#include "Service.h"

#include <QtDBus>

#define NAME_KEY "Name"
#define STATE_KEY "State"
#define TYPE_KEY "Type"
#define SECURITY_KEY "Security"
#define PASSPHRASE_KEY "Passphrase"
#define STRENGTH_KEY "Strength"
#define AUTOCONNECT_KEY "AutoConnect"
#define IPV4_KEY "IPv4"
#define IPV4_CONFIGURATION_KEY "IPv4.Configuration"

typedef QMap<QString, QString> QStringMap;
Q_DECLARE_METATYPE(QStringMap);

// Derived from https://github.com/OSSystems/qconnman/blob/master/common/service.h
Kovan::Network::Network(const QString& path, QObject *parent)
	: ::Network(parent),
	m_service(new Service("net.connman", path, QDBusConnection::systemBus(), this))
{
	qDBusRegisterMetaType<QStringMap>();

	QDBusReply<QVariantMap> reply = m_service->GetProperties();
	m_properties = reply.value();

	QObject::connect(m_service,
		SIGNAL(PropertyChanged(QString, QDBusVariant)),
		SLOT(propertyChanged(QString, QDBusVariant)));
}

QString Kovan::Network::path() const
{
	return m_service->path();
}

QString Kovan::Network::name() const
{
	return m_properties[NAME_KEY].toString();
}

bool Kovan::Network::isHidden() const
{
	return name().isEmpty();
}

QString Kovan::Network::state() const
{
	return m_properties[STATE_KEY].toString();
}

QString Kovan::Network::type() const
{
	return m_properties[TYPE_KEY].toString();
}

QStringList Kovan::Network::security() const
{
	return m_properties[SECURITY_KEY].toStringList();
}

QString Kovan::Network::passphrase() const
{
	return m_properties[PASSPHRASE_KEY].toString();
}

void Kovan::Network::setPassphrase(const QString& passphrase)
{
	QDBusPendingReply<void> reply = m_service->SetProperty(PASSPHRASE_KEY, QDBusVariant(passphrase));
	reply.waitForFinished();
}

quint8 Kovan::Network::strength() const
{
	return m_properties[STRENGTH_KEY].value<quint8>();
}

void Kovan::Network::setAutoConnect(const bool& value)
{
	QDBusPendingReply<void> reply = m_service->SetProperty(AUTOCONNECT_KEY, QDBusVariant(qVariantFromValue(value)));
	reply.waitForFinished();
}

QVariantMap Kovan::Network::ipv4Settings()
{
	return qdbus_cast<QVariantMap>(m_properties[IPV4_KEY]);
}

void Kovan::Network::setIpv4Settings(const QVariantMap& map)
{
	QStringMap settings;
	foreach(const QString &key, map.keys()) settings[key] = map[key].toString();
	QDBusPendingReply<void> reply = m_service->SetProperty(IPV4_CONFIGURATION_KEY, QDBusVariant(qVariantFromValue(settings)));
	reply.waitForFinished();
}

void Kovan::Network::connect()
{
	m_service->Connect();
}

void Kovan::Network::disconnect()
{
	m_service->Disconnect();
}

void Kovan::Network::propertyChanged(const QString& name, const QDBusVariant& value)
{
	m_properties[name] = value.variant();
	if(name == STATE_KEY) {
		qDebug() << "New state: " << m_properties[name];
		emit stateChanged();
	}
	emit propertiesChanged();
}