#ifndef _NETWORKINGPROVIDERDELEGATE_H_
#define _NETWORKINGPROVIDERDELEGATE_H_

class NetworkingProvider;

class NetworkingProviderDelegate
{
public:
	virtual QString networkingProviderNeedsPasswordOfType(NetworkingProvider *networkingProvider, const QString& type) = 0;
	virtual QString networkingProviderNeedsNetworkName(NetworkingProvider *networkingProvider) = 0;
	virtual void networkingProviderReportedError(NetworkingProvider *networkingProvider, const QString& message) = 0;
};

#endif
