#ifndef _KOVANDEVICE_H_
#define _KOVANDEVICE_H_

#include "Device.h"

namespace Kovan
{
	class Device : public ::Device
	{
	public:
		Device();
		~Device();
		
		virtual QString name() const;
		virtual QString version() const;
		
		virtual FilesystemProvider *filesystemProvider() const;
		virtual CommunicationProviderList communicationProviders() const;
		virtual NetworkingProvider *networkingProvider() const;
		virtual BatteryLevelProvider *batteryLevelProvider() const;
		
	private:
		FilesystemProvider *m_filesystemProvider;
		CommunicationProviderList m_communicationProviders;
		NetworkingProvider *m_networkingProvider;
		BatteryLevelProvider *m_batteryLevelProvider;
	};
}

#endif
