#include "Options.h"
#include <QCoreApplication>
#ifdef NETWORK_ENABLED
#include "org_freedesktop_NetworkManager_Connection_Active.h"
#include "NetworkSettingsWidget.h"
#include "NetworkStatusWidget.h"
#include "ui_NetworkSettingsWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "ConnectWidget.h"
#include "ManageNetworksWidget.h"

#include "NetworkManager.h"
#include "SystemUtils.h"
#include "Execute.h"
#include <QMessageBox>
#include <QTimer>
#include <QMovie>
#include <QLabel>
#include <QDebug>
#include <QProcess>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSize>
#include <unistd.h>

#define NM_STATE_CONNECTED_GLOBAL 70;
QString RASPBERRYPI_TYPE_SETTINGS;
QString WOMBAT_NAME;
QString INITIAL_CONNECTION_CONFIG;

NetworkSettingsWidget::NetworkSettingsWidget(Device *device, QWidget *parent)
		: StandardWidget(device, parent), ui(new Ui::NetworkSettingsWidget), enableCoolOffTimer(new QTimer(this)), msgBox(nullptr)
{
	ui->setupUi(this);
	performStandardSetup(tr("Network Settings"));

	bool initializeEventState = getEventModeStateDefault();

	if (initializeEventState) // Event Mode Enabled
	{
		eventModeEnabledState();
	}
	else // Event Mode Disabled
	{
		eventModeDisabledState();
	}

	qDebug() << "After initializeEventState";

	enableCoolOffTimer = new QTimer(this);
	enableCoolOffTimer->setSingleShot(true);

	QObject::connect(ui->ConnectButton, SIGNAL(clicked()), SLOT(connect()));
	QObject::connect(ui->ManageButton, SIGNAL(clicked()), SLOT(manage()));
	QObject::connect(ui->toggleSwitch, SIGNAL(stateChanged(int)), this, SLOT(toggleChanged()));
	QObject::connect(&NetworkManager::ref(),
									 &NetworkManager::stateChangedBandBouncer,
									 this,
									 &NetworkSettingsWidget::onStateChanged);
	NetworkManager::ref().connect(ui->connectionModeSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));

	ui->ConnectButton->setVisible(true);
	ui->ManageButton->setVisible(true);

	QTimer *updateTimer = new QTimer(this);
	QObject::connect(updateTimer, SIGNAL(timeout()), SLOT(updateInformation()));
	updateTimer->start(10000);

	updateInformation();
}

void NetworkSettingsWidget::eventModeEnabledState()
{
	// Basically, turn off all interactive widgets
	ui->connectionModeSelect->clear();
	ui->connectionModeSelect->addItems({"Event Mode"});
	ui->connectionModeSelect->setEnabled(true);
	ui->connectionModeSelect->setCurrentIndex(0);
	ui->toggleSwitch->setEnabled(false);

	ui->ConnectButton->setEnabled(false);
	ui->ManageButton->setEnabled(false);
	ui->state->setText("");
	ui->ssid->setText("");
	ui->password->setText("");
	ui->security->setText("");
}

void NetworkSettingsWidget::eventModeDisabledState()
{
	// Initialize as normal
	getCurrentMode(); // Get current mode

	getWombatName(); // Get Wombat name

	INITIAL_CONNECTION_CONFIG = getConnectionConfig(); // Get initial connection config
	RASPBERRYPI_TYPE_SETTINGS = getRaspberryPiType();

	if (RASPBERRYPI_TYPE_SETTINGS == "3B+") // if RaspberryPi is 3B+
	{

		if (INITIAL_CONNECTION_CONFIG.contains("band=a")) // If currently on 5GHz band
		{
			ui->toggleSwitch->setChecked(true); // 5GHz toggle side
			ui->toggleSwitch->setEnabled(true); // If 3B+, can switch between 2.4GHz and 5GHz
		}
		else if (INITIAL_CONNECTION_CONFIG.contains("band=bg"))
		{
			ui->toggleSwitch->setChecked(false); // 2.4GHz toggle side
			ui->toggleSwitch->setEnabled(true);	 // If 3B+, can switch between 2.4GHz and 5GHz
		}
	}

	else if (RASPBERRYPI_TYPE_SETTINGS == "3B")
	{
		ui->toggleSwitch->setChecked(false); // 2.4GHz toggle side
		ui->toggleSwitch->setEnabled(false); // If 3B, can only use 2.4GHz
	}

	ui->connectionModeSelect->clear();
	ui->connectionModeSelect->addItems({"AP Mode", "Client Mode", "Wifi Off"});
	ui->connectionModeSelect->setCurrentIndex(0);
	ui->connectionModeSelect->setEnabled(true);
}

void NetworkSettingsWidget::getCurrentMode()
{
	QProcess modeProcess;
	QString command = "grep '^MODE' /home/kipr/wombat-os/configFiles/wifiConnectionMode.txt | awk '{print $2}'";

	modeProcess.start("bash", QStringList() << "-c" << command);
	modeProcess.waitForFinished();

	QString output = modeProcess.readAllStandardOutput().trimmed();

	if (!output.isEmpty())
	{
		qDebug() << "CURRENT MODE is set to:" << output;
	}
	else
	{
		qDebug() << "Failed to read MODE.";
	}
}
QString NetworkSettingsWidget::getRaspberryPiType()
{
	QStringList arguments;
	arguments << "-c" << "cat /proc/cpuinfo | grep Revision | awk '{print $3}'";

	QProcess *myProcess = new QProcess(this);
	myProcess->start("/bin/sh", arguments); // Use /bin/sh or /bin/bash to interpret the command
	myProcess->waitForFinished();
	QByteArray output = myProcess->readAllStandardOutput();

	qDebug() << "Revision code output: " << output;
	if (output.trimmed() == "a020d3" || output.trimmed() == "a020d4")
	{
		RASPBERRYPI_TYPE_SETTINGS = "3B+";
	}
	else if (output.trimmed() == "a02082" || output.trimmed() == "a22082" || output.trimmed() == "a32082" || output.trimmed() == "a52082" || output.trimmed() == "a22083")
	{
		RASPBERRYPI_TYPE_SETTINGS = "3B";
	}
	else
	{
		RASPBERRYPI_TYPE_SETTINGS = "Unknown";
	}

	qDebug() << "RASPBERRYPI_TYPE_SETTINGS: " << RASPBERRYPI_TYPE_SETTINGS;
	return RASPBERRYPI_TYPE_SETTINGS;
}

void NetworkSettingsWidget::getWombatName()
{
	QStringList arguments;
	arguments << "-c" << "nmcli -t -f NAME connection show --active | awk '/-wombat/'";

	QProcess *myProcess = new QProcess(this);
	myProcess->start("/bin/sh", arguments); // Use /bin/sh or /bin/bash to interpret the command
	myProcess->waitForFinished();
	QByteArray output = myProcess->readAllStandardOutput();

	qDebug() << "Wombat name output: " << output;

	WOMBAT_NAME = QString(output).trimmed();
}

QString NetworkSettingsWidget::getConnectionConfig()
{
	QString command = QString("cat /etc/NetworkManager/system-connections/%1.nmconnection").arg(WOMBAT_NAME);

	QProcess *myProcess = new QProcess(this);
	myProcess->start("sudo", QStringList() << "/bin/sh" << "-c" << command);
	myProcess->waitForFinished();
	QByteArray output = myProcess->readAllStandardOutput();

	qDebug() << "Connection config output: " << output;

	return QString(output).trimmed();
}

void NetworkSettingsWidget::toggleChanged()
{

	// StandardWidget::disableMenuBar();
	ui->toggleSwitch->setEnabled(false);
	QString currentConfig = NetworkManager::ref().getAPConnectionConfig();
	QString newBand;
	int newChannel;
	qDebug() << "toggle changed";
	qDebug() << "Current Config after toggle click: " << currentConfig;

	if (currentConfig.contains("band=a")) // Currently set to 5GHz band
	{
		if (QMessageBox::question(this, "Change Wifi Band?",
															QString("You are about to change your Wifi Band from 5GHz to 2.4GHz. \n Do you want to continue?"),
															QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
		{

			StandardWidget::enableMenuBar();
			ui->toggleSwitch->setChecked(true); // Keep on 5GHz toggle side
			ui->toggleSwitch->setEnabled(true);

			return;
		}

		newBand = "bg"; // 2.4GHz band
		newChannel = 1;
	}
	else if (currentConfig.contains("band=bg"))
	{
		if (QMessageBox::question(this, "Change Wifi Band?",
															QString("You are about to change your Wifi Band from 2.4GHz to 5GHz. \n Do you want to continue?"),
															QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
		{

			StandardWidget::enableMenuBar();
			ui->toggleSwitch->setChecked(false); // Keep on 2.4GHz toggle side
			ui->toggleSwitch->setEnabled(true);

			return;
		}
		newBand = "a"; // 5GHz band
		newChannel = 36;
	}

	NetworkManager::ref().changeWifiBands(newBand, newChannel);
	rebootBox();
	currentConfig = NetworkManager::ref().getAPConnectionConfig();
	qDebug() << "Updated Config after toggling change: " << currentConfig;
}
void NetworkSettingsWidget::onStateChanged(const QString &oldBand, const QString &newBand, int oldChannel, int newChannel)
{

	// If old Wifi band and channel are not the updated band and channel
	if ((oldBand != newBand) || (oldChannel != newChannel))
	{

		if (msgBox)
		{
			msgBox->hide();
			delete msgBox;
			msgBox = nullptr;
		}

		ui->toggleSwitch->setEnabled(true); // Re-enable the toggle switch
		StandardWidget::enableMenuBar();

		qDebug() << "Wifi band change completed.";
	}
	else
	{
		qDebug() << "Wifi band change failed.";
	}
}
void NetworkSettingsWidget::rebootBox()
{

	if (!msgBox)
	{
		// Create the QMessageBox
		msgBox = new QMessageBox(this);
		msgBox->setWindowTitle("Switch Wifi Bands");
		msgBox->setMaximumSize(500, 480); // Limit the size of the QMessageBox
		msgBox->setStandardButtons(QMessageBox::NoButton);

		// Create QLabel for the GIF
		QLabel *gifLabel = new QLabel();
		gifLabel->setAlignment(Qt::AlignCenter); // Center the GIF label

		// Create QLabel for the message text
		QLabel *messageLabel = new QLabel("Switching Bands Now...");
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
	qDebug() << "Message box displayed, starting wifi band change sequence...";
}
void NetworkSettingsWidget::editWifiConnectionMode(int newMode)
{
	QProcess process;
	QString command = QString("sed -i 's/^MODE.*/MODE %1/' /home/kipr/wombat-os/configFiles/wifiConnectionMode.txt").arg(newMode);

	process.start("bash", QStringList() << "-c" << command);
	process.waitForFinished();

	if (process.exitStatus() == QProcess::NormalExit)
	{
		qDebug() << "Successfully set MODE to:" << newMode;
	}
	else
	{
		qDebug() << "Failed to set MODE.";
	}
}

void NetworkSettingsWidget::indexChanged(int index)
{
	NetworkManager::ref().turnOn();
	editWifiConnectionMode(index);

	if (index == 0) // AP mode
	{

		NetworkManager::ref().enableAP();
		ui->ConnectButton->setEnabled(false);
	}
	else if (index == 1) // Wifi on (client mode)
	{

		if (NetworkManager::ref().currentActiveConnectionName() == NetworkManager::ref().getAPName())
		{
			NetworkManager::ref().disableAP();
		}

		ui->ConnectButton->setEnabled(true);
		ui->state->setText("ON");
	}
	else if (index == 2) // Wifi off
	{
		NetworkManager::ref().turnOff();
		ui->ConnectButton->setEnabled(false);
	}
}

NetworkSettingsWidget::~NetworkSettingsWidget()
{
	if (msgBox)
	{
		msgBox->deleteLater(); // or delete msgBox; if you want to directly delete
	}
	delete ui;
	delete enableCoolOffTimer;
}

void NetworkSettingsWidget::connect() // Connects to network
{
	RootController::ref().presentWidget(new ConnectWidget(device()));
}

void NetworkSettingsWidget::manage() // Forget or add network to history
{
	RootController::ref().presentWidget(new ManageNetworksWidget(device()));
}

void NetworkSettingsWidget::updateInformation()
{
	const bool on = NetworkManager::ref().isOn(); //
	ui->ssid->setText(NetworkManager::ref().currentActiveConnectionName());
	ui->ip->setText(NetworkManager::ref().ip4Address());

	const QString id = device()->id();
	const QString serial = device()->serial();

	if (!on) // wifi is off
	{
		ui->connectionModeSelect->setCurrentText("Wifi Off");
		ui->state->setText("OFF");
		ui->ssid->setText(" ");
		ui->ip->setText(" ");
		ui->password->setText(" ");
	}
	else if (NetworkManager::ref().isActiveConnectionOn() == true) // if there's an active connection
	{

		if (NetworkManager::ref().currentActiveConnectionName() != NetworkManager::ref().getAPName()) // if current mode isn't AP
		{
			ui->connectionModeSelect->setCurrentIndex(1);
			ui->state->setText(on ? tr("ON") : tr("OFF"));
			ui->ssid->setText(NetworkManager::ref().currentActiveConnectionName());
			ui->ip->setText(NetworkManager::ref().ip4Address());
		}
		else // if current mode is AP
		{
			ui->connectionModeSelect->setCurrentIndex(0);
			ui->state->setText(on ? tr("ON") : tr("OFF"));
			ui->ssid->setText(NetworkManager::ref().currentActiveConnectionName());
			ui->ip->setText(NetworkManager::ref().ip4Address());
		}
		ui->password->setText(NetworkManager::ref().activeConnectionPassword());
	}

	Network active = NetworkManager::ref().active();

	ui->security->setText(active.securityString());
}

void NetworkSettingsWidget::stateChanged(const NetworkManager::State &newState, const NetworkManager::State &oldState)
{
	qDebug() << "State Changed to" << newState;
	QTimer::singleShot(300, this, SLOT(updateInformation()));
}

#endif
