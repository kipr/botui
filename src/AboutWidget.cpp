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
        ui->deviceName->setText("Wombat v26.0");
        QProcess proc;
        proc.start("cat /sys/class/net/eth0/carrier");

        proc.waitForFinished();
        QString output = proc.readAllStandardOutput();
        if (output.toInt() == 1){
            QProcess proc;
            proc.start("hostname -I");

            proc.waitForFinished();
            QString output = proc.readAllStandardOutput();
            QStringList list = output.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            ui->WiFiaddr->setText(list[1]);
            ui->LANaddr->setText(list[0]);
        }
        else{
            ui->WiFiaddr->setText("192.168.125.1");
            ui->LANaddr->setText("0.0.0.0");
        }

  
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
