#include "NetworkingProvider.h"
#include "NetworkingProviderDelegate.h"

Network::Network(QObject *parent) : QObject(parent) {}
Network::~Network() {}

Network::Network() {}
Network::Network(const Network&) {}
Network& Network::operator=(Network&) { return *this; }

NetworkingProvider::NetworkingProvider(NetworkingProviderDelegate *delegate) : m_delegate(delegate) {}
NetworkingProvider::~NetworkingProvider() {}

NetworkingProviderDelegate *NetworkingProvider::delegate() const
{
	return m_delegate;
}