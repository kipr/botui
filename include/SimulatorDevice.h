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
		
		virtual NetworkingProvider *networkingProvider() const;
		virtual BatteryLevelProvider *batteryLevelProvider() const;
		
	private:
		BatteryLevelProvider *m_batteryLevelProvider;
		QAbstractItemModel *m_networkItemModel;
	};
}

#endif
