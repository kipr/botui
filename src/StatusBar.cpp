#include "StatusBar.h"

#include "NetworkStatusWidget.h"
#include <QLabel>

QLabel *eventModeLabel;

StatusBar::StatusBar(QWidget *parent)
		: QStatusBar(parent)
{
	setSizeGripEnabled(false);
	eventModeLabel = new QLabel("Event Mode Enabled", this);
	eventModeLabel->setGeometry(332, 0, 800, 25);
	eventModeLabel->setScaledContents(true);
	eventModeLabel->lower();
	eventModeLabel->hide();
}

void StatusBar::loadDefaultWidgets()
{
#ifdef NETWORK_ENABLED
	addPermanentWidget(new NetworkStatusWidget(this));
#endif
}

void StatusBar::addPermanentEventModeLabel()
{

	if (!eventModeLabel)
	{
			eventModeLabel = new QLabel("Event Mode Enabled", this);
		eventModeLabel->setGeometry(332, 0, 800, 25);
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
