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
		virtual bool isTouchscreen() const;
		
		virtual FilesystemProvider *filesystemProvider() const;
		virtual CommunicationProviderList communicationProviders() const;
		virtual NetworkingProvider *networkingProvider() const;
		virtual CompileProvider *compileProvider() const;
		virtual BatteryLevelProvider *batteryLevelProvider() const;
		virtual PackageManagerProvider *packageManagerProvider() const;
		
	private:
		BatteryLevelProvider *m_batteryLevelProvider;
		QAbstractItemModel *m_networkItemModel;
	};
}

#endif
