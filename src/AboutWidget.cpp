#include "AboutWidget.h"
#include "ui_AboutWidget.h"
#include "Device.h"
#include "SystemUtils.h"
#include "NetworkManager.h"
#include "NetworkSettingsWidget.h"
#include "RootController.h"
#include "DeveloperListWidget.h"

#include <QDebug>
#include <QRegularExpression>

AboutWidget::AboutWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::AboutWidget)
{
  ui->setupUi(this);
  // Setup the UI
  performStandardSetup(tr("About"));
  // Version Number
  ui->version->setText("Version 30.0");
  const bool on = NetworkManager::ref().isOn();
  // ui->deviceName->setText(device->name() + " v" + device->version());

  // Display Serial Number
  const QString serial = device->serial();
  ui->deviceName->setText("Wombat-" + serial);

  if (on)
  { // Network Manager is on

    // Check if eth0 is active (/sys/class/net/eth0/carrier will output 1 if eth0 is active and 0 if it is not)
    QStringList arguments;
    arguments << "/sys/class/net/eth0/carrier";

    QProcess *myProcess = new QProcess(parent);
    myProcess->start("cat", arguments);
    myProcess->waitForFinished();
    QByteArray output = myProcess->readAllStandardOutput();
  
    // If eth0 is active
    if (output.at(0) == '1')
    {
      qDebug() << "ENTERED ETHER";

      // Pull network information
      QProcess *myProc = new QProcess(parent);
      QStringList args;
      args << "-I";
      myProc->start("hostname", args);
      myProc->waitForFinished();
      QByteArray output = myProc->readAllStandardOutput();

      // Parse the output and set as text for IP addresses
      QList<QByteArray> list = output.split(' ');
      qDebug() << list;
      ui->ssid->setText(NetworkManager::ref().currentActiveConnectionName());
      ui->password->setText(NetworkManager::ref().activeConnectionPassword());
      ui->WiFiaddr->setText(list[1]);
      ui->LANaddr->setText(list[0]);
    }

    else if (output.at(0) == '0') // ethernet is not active
    {
      qDebug() << "ENTERED NOT ETHER";
      ui->ssid->setText(NetworkManager::ref().currentActiveConnectionName());
      ui->WiFiaddr->setText(NetworkManager::ref().ip4Address());
      ui->password->setText(NetworkManager::ref().activeConnectionPassword());
      ui->LANaddr->setText("0.0.0.0");
    }
  }
  else
  { // Network Manager is off
    ui->ssid->setText("");
    ui->WiFiaddr->setText("");
    ui->password->setText("");
    ui->LANaddr->setText("0.0.0.0");
  }

  connect(ui->developerList, SIGNAL(clicked()), SLOT(developerList()));

}

AboutWidget::~AboutWidget()
{
  delete ui;
}

void AboutWidget::developerList()
{
  RootController::ref().presentWidget(new DeveloperListWidget(device()));
}
