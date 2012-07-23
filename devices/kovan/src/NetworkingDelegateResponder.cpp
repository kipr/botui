#include "NetworkingDelegateResponder.h"
#include "Agent.h"
#include "Network.h"

#include <QDebug>

#define NAME_KEY "Name"
#define PASSWORD_KEY "Passphrase"
#define TYPE_KEY "Type"
#define INVALID_KEY "invalid-key"

using namespace Kovan;


// Loosely derived from https://github.com/OSSystems/qconnman/blob/master/common/agent.cpp
NetworkingDelegateResponder::NetworkingDelegateResponder(NetworkingProvider *networkingProvider, NetworkingProviderDelegate *delegate, QObject *parent)
	: QObject(parent),
	m_networkingProvider(networkingProvider),
	m_delegate(delegate)
{
	new Agent(this);
	QDBusConnection::systemBus().registerObject("/", this);
	QDBusConnection::systemBus().registerService("net.connman.Agent");
}

NetworkingProvider *NetworkingDelegateResponder::networkingProvider() const
{
	return m_networkingProvider;
}

NetworkingProviderDelegate *NetworkingDelegateResponder::delegate() const
{
	return m_delegate;
}

void NetworkingDelegateResponder::Release()
{
	qDebug() << "NetworkingDelegateResponder::Release";
}

QVariantMap NetworkingDelegateResponder::RequestInput(const QDBusObjectPath& path, const QVariantMap& fields)
{
	QVariantMap ret;
	qDebug() << "NetworkingDelegateResponder::RequestInput in" << fields.keys();

	if(fields.contains(NAME_KEY)) {
		QString reply = m_delegate->networkingProviderNeedsNetworkName(m_networkingProvider);
		ret[NAME_KEY] = reply;
	} else if(fields.contains(PASSWORD_KEY)) {
		qDebug() << "Type:" << fields[PASSWORD_KEY].toMap()[TYPE_KEY];
		QString reply = m_delegate->networkingProviderNeedsPasswordOfType(m_networkingProvider, fields[PASSWORD_KEY].toMap()[TYPE_KEY].toString());
		if(reply.isEmpty()) {
			sendErrorReply("net.connman.Agent.Error.Cancel", "");
			return fields;
		}
		ret[PASSWORD_KEY] = reply;
	}

	qDebug() << "NetworkingDelegateResponder::RequestInput out" << ret;
	return ret;
}

void NetworkingDelegateResponder::ReportError(const QDBusObjectPath& path, const QString& error)
{
	qDebug() << "NetworkingDelegateResponder::ReportError" << path.path() << error;
	Network service(path.path(), this);
	if(error == INVALID_KEY) m_delegate->networkingProviderReportedError(m_networkingProvider,
		tr("Unable to connect to %1. The password is invalid.").arg(service.name()));
}