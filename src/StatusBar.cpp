#include "StatusBar.h"

#include "Device.h"
#include "NetworkStatusWidget.h"
#include "TimeWidget.h"
#include "BatteryWidget.h"


StatusBar::StatusBar(QWidget *parent)
	: QStatusBar(parent)
{
	setSizeGripEnabled(false);
}

void StatusBar::loadDefaultWidgets(Device *device)
{
	if(device->batteryLevelProvider())
		addPermanentWidget(new BatteryWidget(device->batteryLevelProvider(), this));

#ifdef NETWORK_ENABLED
	addPermanentWidget(new NetworkStatusWidget(this));
#endif
}
