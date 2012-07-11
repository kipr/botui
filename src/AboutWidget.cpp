#include "AboutWidget.h"
#include "ui_AboutWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include <QDebug>

AboutWidget::AboutWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::AboutWidget),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this))
{
	ui->setupUi(this);
	m_menuBar->addHomeAndBackButtons();
	ui->deviceName->setText(m_device->name() + " " + m_device->version());
	m_menuBar->setTitle("About");
	m_menuBar->addAction("Update");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
}

AboutWidget::~AboutWidget()
{
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}
