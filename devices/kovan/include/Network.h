#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "NetworkingProvider.h"
#include "Service.h"
#include <QDBusVariant>

class QDBusPendingCallWatcher;

// Derived from https://github.com/OSSystems/qconnman/blob/master/common/service.h
namespace Kovan
{
	class Network : public ::Network
	{
	Q_OBJECT
	Q_PROPERTY(QString path READ path)
	Q_PROPERTY(QString name READ name)
	Q_PROPERTY(bool hidden READ isHidden)
	Q_PROPERTY(QString state READ state)
	Q_PROPERTY(QString type READ type)
	Q_PROPERTY(QStringList security READ security)
	Q_PROPERTY(QString passphrase READ passphrase WRITE setPassphrase)
	Q_PROPERTY(quint8 strength READ strength)
	Q_PROPERTY(QVariantMap ipv4Settings READ ipv4Settings WRITE setIpv4Settings)

	public:
		Network(const QString &path, QObject *parent = 0);

		virtual QString path() const;
		virtual QString name() const;
		virtual bool isHidden() const;
		virtual QString state() const;
		virtual QString type() const;
		virtual QStringList security() const;
		virtual QString passphrase() const;
		virtual void setPassphrase(const QString& passphrase);
		virtual quint8 strength() const;

		virtual void setAutoConnect(const bool& value);

		virtual QVariantMap ipv4Settings();
		virtual void setIpv4Settings(const QVariantMap& map);

	public slots:
		virtual void connect();
		virtual void disconnect();

	signals:
		void propertiesChanged();
		void stateChanged();

	private slots:
		void propertyChanged(const QString& name, const QDBusVariant& value);

	private:
		
		net::connman::Service *m_service;
		QVariantMap m_properties;
	};
}

#endif
