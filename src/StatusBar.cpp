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
	qDebug() << "inside addPermanentEventModeLabel";
	eventModeLabel = new QLabel("Event Mode Enabled", this);
	eventModeLabel->setGeometry(300, 0, 800, 25);
	eventModeLabel->setScaledContents(true);
	eventModeLabel->raise();
	eventModeLabel->show();
}

void StatusBar::removePermanentEventModeLabel()
{
	qDebug() << "inside removePermanentEventModeLabel";
	if (eventModeLabel)
	{
		qDebug() << "delete eventModeLabel";
		delete eventModeLabel;
		eventModeLabel = nullptr;
	}
}