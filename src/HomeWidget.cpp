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
#include "ProgramSelectionWidget.h"
#include "Program.h"
#include "LockScreen.h"

HomeWidget::HomeWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::HomeWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Home Page"));
	
	connect(ui->programs, SIGNAL(clicked()), SLOT(programs()));
	connect(ui->motorsSensors, SIGNAL(clicked()), SLOT(motorsSensors()));
	connect(ui->settings, SIGNAL(clicked()), SLOT(settings()));
	connect(ui->about, SIGNAL(clicked()), SLOT(about()));
	
	QAction *lock = m_menuBar->addAction("Lock Screen");
	connect(lock, SIGNAL(activated()), SLOT(lock()));
	
	ui->programs->setEnabled(m_device->filesystemProvider());
}

HomeWidget::~HomeWidget()
{
	delete ui;
}

void HomeWidget::programs()
{
	RootController::ref().presentWidget(Program::instance()->isRunning()
		? (QWidget *)new ProgramSelectionWidget(m_device)
		: (QWidget *)new ProgramsWidget(m_device));
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