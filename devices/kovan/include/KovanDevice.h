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
		virtual QString id() const;
		virtual bool isTouchscreen() const;
		
		virtual CompileProvider *compileProvider() const;
		virtual BatteryLevelProvider *batteryLevelProvider() const;
		virtual SettingsProvider *settingsProvider() const;
		virtual ButtonProvider *buttonProvider() const;
		
	private:
		CompileProvider *m_compileProvider;
		Connman *m_networkingProvider;
		BatteryLevelProvider *m_batteryLevelProvider;
		SettingsProvider *m_settingsProvider;
		ButtonProvider *m_buttonProvider;
	};
}

#endif
