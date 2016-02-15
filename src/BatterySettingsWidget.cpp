#include "BatterySettingsWidget.h"
#include "ui_BatterySettingsWidget.h"

#include <QDebug>

#include "Device.h"
#include "SettingsProvider.h"

#define BATTERY_TYPE_KEY "battery_type"

BatterySettingsWidget::BatterySettingsWidget(Device *device, QWidget *const parent)
  : StandardWidget(device, parent)
  , ui(new Ui::BatterySettingsWidget)
{
  ui->setupUi(this);
  performStandardSetup(tr("Battery Settings Widget"));
  
  // TODO: Populate combo box with availalbe battery types
  
  // Set current battery type
  const SettingsProvider *const settingsProvider = device->settingsProvider();
  if(settingsProvider) {
    const int currType = settingsProvider->value(BATTERY_TYPE_KEY, 0).toInt();
    if(currType >= 0 && currType <= 2)
      ui->batteryType->setCurrentIndex(currType);
  }
  
  connect(ui->batteryType, SIGNAL(currentIndexChanged(int)), SLOT(typeChanged(int)));
}

BatterySettingsWidget::~BatterySettingsWidget()
{
  delete ui;
}

void BatterySettingsWidget::typeChanged(int type)
{
  SettingsProvider *const settingsProvider = device()->settingsProvider();
  if(!settingsProvider)
    return;
  
  settingsProvider->setValue(BATTERY_TYPE_KEY, type);
  settingsProvider->sync();
}