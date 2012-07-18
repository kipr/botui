#include "OtherNetworkWidget.h"
#include "ui_OtherNetworkWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "KeyboardDialog.h"
#include <QDebug>

OtherNetworkWidget::OtherNetworkWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::OtherNetworkWidget),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this)),
	m_ssid(new KeyboardDialog("Network Name")),
	m_password(new KeyboardDialog("Password"))
{
	ui->setupUi(this);
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle("Other Network");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
	
	ui->ssid->setInputProvider(m_ssid);
	ui->password->setInputProvider(m_password);
}

OtherNetworkWidget::~OtherNetworkWidget()
{
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
	
	delete m_ssid;
	delete m_password;
}
