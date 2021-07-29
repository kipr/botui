#include "SettingsWidget.h"
#include "ui_SettingsWidget.h"
#include "SettingsProvider.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "Calibrate.h"
#include "Options.h"
#include "NotYetImplementedDialog.h"

#include "ChannelConfigurationsWidget.h"
#include "LanguageWidget.h"
#include "AdvancedSettingsWidget.h"
#include "WallabyUpdateWidget.h"
#include "CameraSettingsWidget.h"
#include "BackupWidget.h"

#include <QDebug>

SettingsWidget::SettingsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::SettingsWidget)
{
	ui->setupUi(this);
        performStandardSetup(tr("Settings"));

	const SettingsProvider *const settingsProvider = device->settingsProvider();
	  if(settingsProvider) {
	    const bool hideUI = settingsProvider->value("hideUI").toBool();   
	    ui->hideUi->setVisible(hideUI);
	  }

	connect(ui->channels, SIGNAL(clicked()), SLOT(channels()));
	connect(ui->calibrate, SIGNAL(clicked()), SLOT(calibrate()));
	connect(ui->language, SIGNAL(clicked()), SLOT(language()));
        connect(ui->update, SIGNAL(clicked()), SLOT(update()));
        connect(ui->hideUi, SIGNAL(clicked()), SLOT(hideUi()));
        connect(ui->cameraView, SIGNAL(clicked()), SLOT(cameraView()));
        connect(ui->backup, SIGNAL(clicked()), SLOT(backup()));
        connect(ui->advanced, SIGNAL(clicked()), SLOT(advanced()));
        connect(ui->millennialSnowflake, SIGNAL(clicked()), SLOT(millennialSnowflake()));
	
}

SettingsWidget::~SettingsWidget()
{
	delete ui;
}

void SettingsWidget::advanced()
{
        RootController::ref().presentWidget(new AdvancedSettingsWidget(device()));
}

void SettingsWidget::channels()
{
	RootController::ref().presentWidget(new ChannelConfigurationsWidget(device()));
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

void SettingsWidget::cameraView()
{
  RootController::ref().presentWidget(new CameraSettingsWidget(device()));
}
void SettingsWidget::backup()
{
  RootController::ref().presentWidget(new BackupWidget(device()));
}
void SettingsWidget::millennialSnowflake()
{

  if (ui->millennialSnowflake.text() == "Make a snowflake") {
   ui->millennialSnowflake.setText("Make a snowflake");

  }

  else {
      ui->millennialSnowflake.setText("Don't make a snowflake");
  }


}
