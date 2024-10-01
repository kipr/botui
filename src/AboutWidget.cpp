#include "AboutWidget.h"
#include "ui_AboutWidget.h"
#include "Device.h"
#include "SystemUtils.h"
#include "NetworkManager.h"
#include "NetworkSettingsWidget.h"
#include "RootController.h"
#include "DeveloperListWidget.h"
#include <QProcess>
#include <QDebug>
#include <QRegularExpression>

AboutWidget::AboutWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::AboutWidget)
{
  ui->setupUi(this);
  // Setup the UI
  performStandardSetup(tr("About"));

  //Set up emission signals for Event Mode enabled/disabled
  setupConnections(this);

  //Event Mode persistent state check
  getEventModeState();
  
  const bool on = NetworkManager::ref().isOn();
  // Version Number
  ui->version->setText(device->name() + " v" + device->version());

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
  connect(ui->toggleSwitch, SIGNAL(stateChanged(int)), this, SLOT(eventModeBackground(int)));
}

AboutWidget::~AboutWidget()
{
  delete ui;
}

void AboutWidget::getEventModeState()
{
  QProcess eventModeProcess;
  QString command = "grep '^EVENT_MODE' /home/kipr/wifiConnectionMode.txt | awk '{print $2}'";

  eventModeProcess.start("bash", QStringList() << "-c" << command);
  eventModeProcess.waitForFinished();

  QString output = eventModeProcess.readAllStandardOutput().trimmed();

  if (!output.isEmpty())
  {
    qDebug() << "CURRENT EVENT_MODE is set to:" << output;
    if (output == "true")
    {
      ui->toggleSwitch->setChecked(true);
    }
    else
    {
      ui->toggleSwitch->setChecked(false);
    }
  }
  else
  {
    qDebug() << "Failed to read EVENT_MODE.";
  }
}

void AboutWidget::setEventModeState(QString newState)
{
  QProcess process;
  QString command = QString("sed -i 's/^EVENT_MODE.*/EVENT_MODE %1/' /home/kipr/wifiConnectionMode.txt").arg(newState);

  process.start("bash", QStringList() << "-c" << command);
  process.waitForFinished();

  if (process.exitStatus() == QProcess::NormalExit)
  {
    qDebug() << "Successfully set EVENT_MODE to:" << newState;
  }
  else
  {
    qDebug() << "Failed to set EVENT_MODE.";
  }
}

void AboutWidget::eventModeBackground(int checked)
{

  qDebug() << "Event Mode Background toggled";
  qDebug() << "Checked: " << checked;
  if (checked == 2) //Enable Event Mode
  {

    setEventModeState("true");
    emit eventModeEnabled();
    NetworkManager::ref().deactivateAP();
  }
  else //Disable Event Mode
  {
    setEventModeState("false");
    emit eventModeDisabled();
    NetworkManager::ref().enableAP();

  }
}

void AboutWidget::developerList()
{
  RootController::ref().presentWidget(new DeveloperListWidget(device()));
}
