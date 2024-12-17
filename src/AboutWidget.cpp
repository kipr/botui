#include "AboutWidget.h"
#include "ui_AboutWidget.h"
#include "Device.h"
#include "SystemUtils.h"
#include "NetworkManager.h"
#include "NetworkSettingsWidget.h"
#include "RootController.h"
#include "DeveloperListWidget.h"
#include <QProcess>
#include <QMovie>
#include <QDebug>
#include <QRegularExpression>
#include <QTimer>
#include <unistd.h>
AboutWidget::AboutWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::AboutWidget), msgBox(nullptr)
{
  ui->setupUi(this);
  // Setup the UI
  performStandardSetup(tr("About"));

  // Set up emission signals for Event Mode enabled/disabled
  setupConnections(this);

  // Event Mode persistent state check
  bool eventModeState = getEventModeState();

  if (eventModeState)
  {
    eventModeBackground(2);
  }

  QString piType = getRaspberryPiType();

  ui->piType->setText(piType);

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
  if (msgBox)
  {
    msgBox->deleteLater(); // or delete msgBox; if you want to directly delete
  }
  delete ui;
}

void AboutWidget::rebootBox()
{
  qDebug() << "In rebootBox()";

  if (!msgBox)
  {
    // Create the QMessageBox
    msgBox = new QMessageBox(this);
    msgBox->setWindowTitle("Switch Event Mode");
    msgBox->setMaximumSize(500, 480); // Limit the size of the QMessageBox
    msgBox->setStandardButtons(QMessageBox::NoButton);

    // Create QLabel for the GIF
    QLabel *gifLabel = new QLabel();
    gifLabel->setAlignment(Qt::AlignCenter); // Center the GIF label

    // Create QLabel for the message text
    QLabel *messageLabel = new QLabel("Switching Event Mode Now...");
    messageLabel->setAlignment(Qt::AlignCenter); // Center the message label

    // Create a container widget and a new vertical layout
    QWidget *container = new QWidget();
    QVBoxLayout *vLayout = new QVBoxLayout(container);

    // Add the GIF label and message label to the vertical layout
    vLayout->addWidget(gifLabel);
    vLayout->addWidget(messageLabel);

    // Adjust the vertical layout spacing and margins
    vLayout->setSpacing(10);
    vLayout->setContentsMargins(10, 10, 10, 10);

    // Set the layout of the container
    container->setLayout(vLayout);

    // Access the internal layout of the QMessageBox
    QGridLayout *msgBoxLayout = qobject_cast<QGridLayout *>(msgBox->layout());
    if (msgBoxLayout)
    {
      msgBoxLayout->addWidget(container, 0, 0, 1, msgBoxLayout->columnCount());
    }
    else
    {
      qDebug() << "msgBoxLayout is nullptr!"; // Debugging message if layout is nullptr
    }

    // Setup and start the GIF movie
    QMovie *movie = new QMovie("://qml/botguy_noMargin.gif");
    movie->setScaledSize(QSize(200, 240));
    gifLabel->setMovie(movie);
    movie->start();

    // Show the QMessageBox non-blocking
    msgBox->setText(""); // Hide the default text to avoid duplication
  }
  msgBox->show();

  // Debug information
  qDebug() << "Message box displayed, starting event mode change sequence...";
}

QString AboutWidget::getRaspberryPiType()
{
  QProcess process;
  QString command = "awk '/Revision/ {print $3}' /proc/cpuinfo"; // Corrected command syntax

  process.start("bash", QStringList() << "-c" << command);
  process.waitForFinished();
  QByteArray output = process.readAllStandardOutput(); // Fixed this line to use process directly

  QString piType;
  if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) // Check exit code
  {
    qDebug() << "Successfully got Raspberry Pi Type:" << output.trimmed(); // Trim output to remove whitespace

    if (output.trimmed() == "a020d3" || output.trimmed() == "a020d4")
    {
      piType = "3B+";
    }
    else if (output.trimmed() == "a02082" || output.trimmed() == "a22082" || output.trimmed() == "a32082" || output.trimmed() == "a52082" || output.trimmed() == "a22083")
    {
      piType = "3B";
    }
    else
    {
      piType = "Unknown";
    }
  }
  else
  {
    qDebug() << "Failed to get Raspberry Pi type. Exit Code:" << process.exitCode();
  }

  return piType; // Convert QByteArray to QString and trim
}

bool AboutWidget::getEventModeState()
{
  QProcess eventModeProcess;
  QString command = "grep '^EVENT_MODE' /home/kipr/wombat-os/configFiles/wifiConnectionMode.txt | awk '{print $2}'";

  eventModeProcess.start("bash", QStringList() << "-c" << command);
  eventModeProcess.waitForFinished();

  QString output = eventModeProcess.readAllStandardOutput().trimmed();

  if (!output.isEmpty())
  {
    qDebug() << "CURRENT EVENT_MODE is set to:" << output;
    if (output == "true")
    {
      ui->toggleSwitch->setChecked(true);
      return true;
    }
    else
    {
      ui->toggleSwitch->setChecked(false);
      return false;
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
  QString command = QString("sed -i 's/^EVENT_MODE.*/EVENT_MODE %1/' /home/kipr/wombat-os/configFiles/wifiConnectionMode.txt").arg(newState);

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

  ui->toggleSwitch->setEnabled(false);
  rebootBox();
  if (checked == 2) // Enable Event Mode
  {

    setEventModeState("true");
    emit eventModeEnabled();
    NetworkManager::ref().deactivateAP();
  }
  else // Disable Event Mode
  {
    setEventModeState("false");
    emit eventModeDisabled();
    NetworkManager::ref().enableAP();
  }

  QTimer::singleShot(3000, this, [this]()
                     {
        if (msgBox)
        {
            msgBox->hide();
            delete msgBox;
            msgBox = nullptr;
        }
         ui->toggleSwitch->setEnabled(true); });
}

void AboutWidget::developerList()
{
  RootController::ref().presentWidget(new DeveloperListWidget(device()));
}
