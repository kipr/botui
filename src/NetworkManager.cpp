#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
	
}

NetworkManager::~NetworkManager()
{
	
}

const NetworkList &NetworkManager::networks() const
{
	return m_networks;
}