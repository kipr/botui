#include "StandardWidget.h"
#include "AboutWidget.h"

#include "MenuBar.h"
#include "StatusBar.h"
#include "Device.h"
#include <QLayout>
#include <QLabel>
bool eventModeEnabled;

StandardWidget::StandardWidget(Device *device, QWidget *parent)
		: QWidget(parent),
			m_device(device),
			m_menuBar(new MenuBar(this)),
			m_statusBar(0)
{
}

StandardWidget::~StandardWidget()
{
	delete m_menuBar;
	delete m_statusBar;
}

void StandardWidget::disableMenuBar()
{
	m_menuBar->setEnabled(false);
}

bool StandardWidget::getEventModeStateDefault()
{
	QProcess eventModeProcess;
	QString command = "grep '^EVENT_MODE' /home/kipr/wombat-os/configFiles/wifiConnectionMode.txt | awk '{print $2}'";

	eventModeProcess.start("bash", QStringList() << "-c" << command);
	eventModeProcess.waitForFinished();

	QString output = eventModeProcess.readAllStandardOutput().trimmed();

	if (!output.isEmpty())
	{

		if (output == "true")
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		qDebug() << "Failed to read EVENT_MODE.";
	}
}
void StandardWidget::enableMenuBar()
{
	m_menuBar->setEnabled(true);
}

void StandardWidget::setupConnections(AboutWidget *aboutWidget)
{
	connect(aboutWidget, &AboutWidget::eventModeEnabled, this, &StandardWidget::addEventModeLabel);
	connect(aboutWidget, &AboutWidget::eventModeDisabled, this, &StandardWidget::removeEventModeLabel);
}

void StandardWidget::reloadStatusBar()
{

	if (!m_statusBar)
	{
		m_statusBar = new StatusBar(this);
		m_statusBar->loadDefaultWidgets(m_device);
		if (layout())
			layout()->addWidget(m_statusBar);

		if (eventModeEnabled)
		{
			m_statusBar->addPermanentEventModeLabel();
		}
		else if (eventModeEnabled == false)
		{
			m_statusBar->removePermanentEventModeLabel();
		}
	}
}
void StandardWidget::performStandardSetup(const QString &title, const bool &statusBar)
{
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle(title);
	if (layout())
		layout()->setMenuBar(m_menuBar);

	if (!statusBar)
		return;
	m_statusBar = new StatusBar(this);
	m_statusBar->loadDefaultWidgets(m_device);
	if (layout())
		layout()->addWidget(m_statusBar);

	if (eventModeEnabled)
	{
		m_statusBar->addPermanentEventModeLabel();
	}
	else if (eventModeEnabled == false)
	{
		m_statusBar->removePermanentEventModeLabel();
	}
}

void StandardWidget::deleteStatusBar()
{
	if (m_statusBar)
	{
		delete m_statusBar;
		m_statusBar = nullptr;
	}
}

void StandardWidget::addEventModeLabel()
{
	qDebug() << "inside addEventModeLabel, triggering StatusBar addPermanentEventModeLabel";
	eventModeEnabled = true;
	if (m_statusBar)
	{
		m_statusBar->addPermanentEventModeLabel();
	}
}

void StandardWidget::removeEventModeLabel()
{
	qDebug() << "inside removeEventModeLabel, triggering StatusBar removePermanentEventModeLabel";
	eventModeEnabled = false;
	if (m_statusBar)
	{
		m_statusBar->removePermanentEventModeLabel();
	}
}
bool StandardWidget::isEventModeEnabled()
{
	if (eventModeEnabled)
	{
		return true;
	}
	else
	{
		return false;
	}
}
Device *StandardWidget::device() const
{
	return m_device;
}

MenuBar *StandardWidget::menuBar() const
{
	return m_menuBar;
}

StatusBar *StandardWidget::statusBar() const
{
	return m_statusBar;
}