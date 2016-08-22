#include "SettingsWidget.h"
#include "ui_SettingsWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "Calibrate.h"
#include "Options.h"
#include "NotYetImplementedDialog.h"

#include "NetworkSettingsWidget.h"
#include "CommunicationSettingsWidget.h"
#include "ChannelConfigurationsWidget.h"
#include "GuiSettingsWidget.h"
#include "LanguageWidget.h"
#include "WallabyUpdateWidget.h"
#include "BatterySettingsWidget.h"
#include "CameraSettingsWidget.h"

#include <QDebug>

SettingsWidget::SettingsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::SettingsWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Settings"));

  #ifdef NETWORK_ENABLED
  ui->network->setEnabled(true);
  #else
  ui->network->setEnabled(false);
  #endif

	connect(ui->network, SIGNAL(clicked()), SLOT(network()));
	connect(ui->comm, SIGNAL(clicked()), SLOT(comm()));
	connect(ui->channels, SIGNAL(clicked()), SLOT(channels()));
	connect(ui->gui, SIGNAL(clicked()), SLOT(gui()));
	connect(ui->calibrate, SIGNAL(clicked()), SLOT(calibrate()));
	connect(ui->language, SIGNAL(clicked()), SLOT(language()));
  connect(ui->update, SIGNAL(clicked()), SLOT(update()));
  connect(ui->hideUi, SIGNAL(clicked()), SLOT(hideUi()));
  connect(ui->battery, SIGNAL(clicked()), SLOT(battery()));
  connect(ui->cameraView, SIGNAL(clicked()), SLOT(cameraView()));

	//TODO show buttons once the widgets are fixed
	//ui->network->setVisible(false);
	ui->comm->setVisible(false);
}

SettingsWidget::~SettingsWidget()
{
	delete ui;
}

void SettingsWidget::network()
{
#ifdef NETWORK_ENABLED
  RootController::ref().presentWidget(new NetworkSettingsWidget(device()));
#endif
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

void SettingsWidget::language()
{
  RootController::ref().presentWidget(new LanguageWidget(device()));
}

void SettingsWidget::update()
{
  WallabyUpdateWidget *const updateWidget = new WallabyUpdateWidget(device());
  RootController::ref().presentWidget(updateWidget);
  updateWidget->refresh();
}

void SettingsWidget::hideUi()
{
  RootController::ref().minimize();
}

void SettingsWidget::battery()
{
  RootController::ref().presentWidget(new BatterySettingsWidget(device()));
}

void SettingsWidget::cameraView()
{
  RootController::ref().presentWidget(new CameraSettingsWidget(device()));
}
