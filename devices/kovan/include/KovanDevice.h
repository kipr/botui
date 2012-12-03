#ifndef _KOVANDEVICE_H_
#define _KOVANDEVICE_H_

#include "Device.h"

namespace Kovan
{
	class Connman;
	
	class Device : public ::Device
	{
	public:
		Device();
		~Device();
		
		virtual QString name() const;
		virtual QString version() const;
		virtual bool isTouchscreen() const;
		
		virtual FilesystemProvider *filesystemProvider() const;
		virtual CompileProvider *compileProvider() const;
		virtual CommunicationProviderList communicationProviders() const;
		virtual BatteryLevelProvider *batteryLevelProvider() const;
		virtual SettingsProvider *settingsProvider() const;
		virtual ButtonProvider *buttonProvider() const;
		
	private:
		FilesystemProvider *m_filesystemProvider;
		CompileProvider *m_compileProvider;
		CommunicationProviderList m_communicationProviders;
		Connman *m_networkingProvider;
		BatteryLevelProvider *m_batteryLevelProvider;
		SettingsProvider *m_settingsProvider;
		ButtonProvider *m_buttonProvider;
	};
}

#endif
