#include "StatusBar.h"

#include "Device.h"
#include "NetworkStatusWidget.h"
#include "TimeWidget.h"
#include "BatteryWidget.h"
QLabel *eventModeLabel;

StatusBar::StatusBar(QWidget *parent)
		: QStatusBar(parent)
{
	setSizeGripEnabled(false);
	eventModeLabel = new QLabel("Event Mode Enabled", this);
	eventModeLabel->setGeometry(300, 0, 800, 25);
	eventModeLabel->setScaledContents(true);
	eventModeLabel->lower();
	eventModeLabel->hide();
}

void StatusBar::loadDefaultWidgets(Device *device)
{
	if (device->batteryLevelProvider())
		addPermanentWidget(new BatteryWidget(device->batteryLevelProvider(), this));

#ifdef NETWORK_ENABLED
	addPermanentWidget(new NetworkStatusWidget(this));
#endif
}

void StatusBar::addPermanentEventModeLabel()
{

	if (!eventModeLabel)
	{
			eventModeLabel = new QLabel("Event Mode Enabled", this);
		eventModeLabel->setGeometry(340, 0, 800, 25);
		eventModeLabel->setScaledContents(true);
		eventModeLabel->lower();
		eventModeLabel->show();
	}
	else
	{
	
		eventModeLabel->show();
	}
}

void StatusBar::removePermanentEventModeLabel()
{

	if (eventModeLabel)
	{
		delete eventModeLabel;
		eventModeLabel = nullptr;
	}
}