#ifndef _SIMULATORDEVICE_H_
#define _SIMULATORDEVICE_H_

#include "Device.h"

namespace Simulator
{
	class Device : public ::Device
	{
	public:
		Device();
		~Device();
		
		virtual QString name() const;
		virtual QString version() const;
		virtual QString id() const;
		virtual bool isTouchscreen() const;
		
		virtual ArchivesManager *archivesManager() const;
		virtual NetworkingProvider *networkingProvider() const;
		virtual CompileProvider *compileProvider() const;
		virtual BatteryLevelProvider *batteryLevelProvider() const;
		virtual SettingsProvider *settingsProvider() const;
		virtual ButtonProvider *buttonProvider() const;
		
	private:
		BatteryLevelProvider *m_batteryLevelProvider;
		QAbstractItemModel *m_networkItemModel;
	};
}

#endif
