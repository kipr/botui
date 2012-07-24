#include "HomeWidget.h"

#include "ui_HomeWidget.h"
#include "MenuBar.h"
#include "StatusBar.h"
#include "RootController.h"
#include "SettingsWidget.h"
#include "AboutWidget.h"
#include "UnderConstructionWidget.h"
#include "MotorsSensorsWidget.h"
#include "ProgramsWidget.h"
#include "Device.h"
#include "StatusBar.h"
#include "LockScreen.h"

HomeWidget::HomeWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::HomeWidget),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this))
{
	ui->setupUi(this);
	QAction *lock = m_menuBar->addAction("Lock Screen");
	
	m_menuBar->setTitle("Home Page");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
	
	connect(ui->programs, SIGNAL(clicked()), SLOT(programs()));
	connect(ui->motorsSensors, SIGNAL(clicked()), SLOT(motorsSensors()));
	connect(ui->settings, SIGNAL(clicked()), SLOT(settings()));
	connect(ui->about, SIGNAL(clicked()), SLOT(about()));
	connect(lock, SIGNAL(activated()), SLOT(lock()));
	
	ui->programs->setEnabled(m_device->filesystemProvider());
}

HomeWidget::~HomeWidget()
{
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}

void HomeWidget::programs()
{
	RootController::ref().presentWidget(new ProgramsWidget(m_device));
}

void HomeWidget::motorsSensors()
{
	RootController::ref().presentWidget(new MotorsSensorsWidget(m_device));
}

void HomeWidget::settings()
{
	RootController::ref().presentWidget(new SettingsWidget(m_device));
}

void HomeWidget::about()
{
	RootController::ref().presentWidget(new AboutWidget(m_device));
}

void HomeWidget::lock()
{
	LockScreen::lock();
}