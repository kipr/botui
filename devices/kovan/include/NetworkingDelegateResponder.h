#ifndef _NETWORKINGDELEGATERESPONDER_H_
#define _NETWORKINGDELEGATERESPONDER_H_

#include <QObject>
#include <QDBusObjectPath>
#include <QVariantMap>
#include <QDBusContext>

#include "NetworkingProviderDelegate.h"

namespace Kovan
{
	// Loosely derived from https://github.com/OSSystems/qconnman/blob/master/common/agent.h
	class NetworkingDelegateResponder : public QObject, protected QDBusContext
	{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "net.connman.Agent")
	Q_PROPERTY(NetworkingProviderDelegate *delegate READ delegate)

	public:
		NetworkingDelegateResponder(NetworkingProvider *networkingProvider, NetworkingProviderDelegate *delegate, QObject *parent = 0);

		NetworkingProvider *networkingProvider() const;
		NetworkingProviderDelegate *delegate() const;
		
	public slots:
		void Release();
		QVariantMap RequestInput(const QDBusObjectPath& path, const QVariantMap& fields);
		void ReportError(const QDBusObjectPath& path, const QString& error);
		
	private:
		NetworkingProvider *m_networkingProvider;
		NetworkingProviderDelegate *m_delegate;
	};
}

#endif
