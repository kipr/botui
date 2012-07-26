#include "StandardWidget.h"

#include "MenuBar.h"
#include "StatusBar.h"
#include "Device.h"
#include <QLayout>

StandardWidget::StandardWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this))
{

}

StandardWidget::~StandardWidget()
{
	delete m_menuBar;
	delete m_statusBar;
}

void StandardWidget::performStandardSetup(const QString& title)
{
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle(title);
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
}