#include "AboutWidget.h"
#include "ui_AboutWidget.h"
#include "Device.h"
#include "SystemUtils.h"

#include <QDebug>

AboutWidget::AboutWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::AboutWidget)
{
	ui->setupUi(this);
	const QString serial = device->serial();
        ui->wombatSerial->setText(serial);
        //ui->deviceName->setText(device->name() + " v" + device->version());
        ui->deviceName->setText("Wombat v25.6");
  
#ifdef WALLABY
  const QString id = device-> id();
  if(!id.isEmpty()) {
    const QString password = SystemUtils::sha256(id).left(6) + "00";
    const QString ssid = device->serial() + "-wombat";
    ui->ssid->setText(ssid);
    ui->password->setText(password);
  }
#else
  ui->wifiBox->hide();
#endif
	performStandardSetup(tr("About"));
}

AboutWidget::~AboutWidget()
{
	delete ui;
}
