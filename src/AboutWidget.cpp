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

        //Version Number
        ui->version->setText("Version 27.0 (Quark)");

        //ui->deviceName->setText(device->name() + " v" + device->version());

        //Display Serial Number
        const QString serial = device->serial();
        ui->deviceName->setText("Wombat-" + serial);


        //Check if eth0 is active (/sys/class/net/eth0/carrier will output 1 if eth0 is active and 0 if it is not)
        QProcess proc;
        proc.start("cat /sys/class/net/eth0/carrier");
        proc.waitForFinished();
        QString output = proc.readAllStandardOutput();

        //If eth0 is active
        if (output.toInt() == 1){
            //Pull network information
            QProcess proc;
            proc.start("hostname -I");
            proc.waitForFinished();
            QString output = proc.readAllStandardOutput();

            //Parse the output and set as text for IP addresses
            QStringList list = output.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            ui->WiFiaddr->setText(list[1]);
            ui->LANaddr->setText(list[0]);
        }
        else{
            ui->WiFiaddr->setText("192.168.125.1");
            ui->LANaddr->setText("0.0.0.0");
        }

  //Old
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

        //Setup the UI
	performStandardSetup(tr("About"));
}

AboutWidget::~AboutWidget()
{
	delete ui;
}
