#include "SettingsWidget.h"
#include "ui_SettingsWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"

#include "NetworkSettingsWidget.h"
#include "KissIdeSettingsWidget.h"

#include <QDebug>

SettingsWidget::SettingsWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::SettingsWidget),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this))
{
	ui->setupUi(this);
	
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle("Settings");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
	
	ui->networkSettings->setEnabled(m_device->networkingProvider());
	connect(ui->networkSettings, SIGNAL(clicked()), SLOT(network()));
	connect(ui->kissIdeSettings, SIGNAL(clicked()), SLOT(kissIde()));
}

SettingsWidget::~SettingsWidget()
{
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}

void SettingsWidget::network()
{
	RootController::ref().presentWidget(new NetworkSettingsWidget(m_device));
}

void SettingsWidget::kissIde()
{
	RootController::ref().presentWidget(new KissIdeSettingsWidget(m_device));
}