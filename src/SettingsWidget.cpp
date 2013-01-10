#include "SettingsWidget.h"
#include "ui_SettingsWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "Calibrate.h"
#include "NotYetImplementedDialog.h"

#include "NetworkSettingsWidget.h"
#include "CommunicationSettingsWidget.h"
#include "ChannelConfigurationsWidget.h"
#include "GuiSettingsWidget.h"

#include <QDebug>

SettingsWidget::SettingsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::SettingsWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Settings"));
	
	// ui->networkSettings->setEnabled(m_device->networkingProvider());
	
	connect(ui->network, SIGNAL(clicked()), SLOT(network()));
	connect(ui->comm, SIGNAL(clicked()), SLOT(comm()));
	connect(ui->channels, SIGNAL(clicked()), SLOT(channels()));
	connect(ui->gui, SIGNAL(clicked()), SLOT(gui()));
	connect(ui->calibrate, SIGNAL(clicked()), SLOT(calibrate()));
}

SettingsWidget::~SettingsWidget()
{
	delete ui;
}

void SettingsWidget::network()
{
	RootController::ref().presentWidget(new NetworkSettingsWidget(device()));
}

void SettingsWidget::comm()
{
	RootController::ref().presentWidget(new CommunicationSettingsWidget(device()));
}

void SettingsWidget::channels()
{
	RootController::ref().presentWidget(new ChannelConfigurationsWidget(device()));
}

void SettingsWidget::gui()
{
	RootController::ref().presentWidget(new GuiSettingsWidget(device()));
}

void SettingsWidget::calibrate()
{
	Calibrate::calibrate();
}