#include "AdvancedSettingsWidget.h"
#include "ui_AdvancedSettingsWidget.h"
#include "SettingsProvider.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "Calibrate.h"
#include "Options.h"
#include "NotYetImplementedDialog.h"

#include "NetworkSettingsWidget.h"
#include "FactoryWidget.h"
#include "GuiSettingsWidget.h"
#include "BatterySettingsWidget.h"

#include <QDebug>

AdvancedSettingsWidget::AdvancedSettingsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
        ui(new Ui::AdvancedSettingsWidget)
{
	ui->setupUi(this);
        performStandardSetup(tr("Advanced Settings"));

  #ifdef NETWORK_ENABLED
  ui->network->setEnabled(true);
  #else
  ui->network->setEnabled(false);
  #endif

	connect(ui->network, SIGNAL(clicked()), SLOT(network()));
        connect(ui->factory, SIGNAL(clicked()), SLOT(factory()));
	connect(ui->gui, SIGNAL(clicked()), SLOT(gui()));
        connect(ui->battery, SIGNAL(clicked()), SLOT(battery()));
	
}

AdvancedSettingsWidget::~AdvancedSettingsWidget()
{
	delete ui;
}

void AdvancedSettingsWidget::network()
{
#ifdef NETWORK_ENABLED
  RootController::ref().presentWidget(new NetworkSettingsWidget(device()));
#endif
}

void AdvancedSettingsWidget::gui()
{
	RootController::ref().presentWidget(new GuiSettingsWidget(device()));
}

void AdvancedSettingsWidget::battery()
{
  RootController::ref().presentWidget(new BatterySettingsWidget(device()));
}

void AdvancedSettingsWidget::factory()
{
  RootController::ref().presentWidget(new FactoryWidget(device()));
}
