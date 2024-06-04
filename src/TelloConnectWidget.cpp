#include "Options.h"
#include <QCoreApplication>
#ifdef NETWORK_ENABLED
#include "org_freedesktop_NetworkManager_Connection_Active.h"
#include "TelloConnectWidget.h"
// #include "NetworkSettingsWidget.h"
#include "NetworkStatusWidget.h"
#include "ui_TelloConnectWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "TelloNetworksWidget.h"
#include "ManageNetworksWidget.h"
#include "NetworkManager.h"
#include "SystemUtils.h"

#include <QTimer>

#include <QDebug>
QString currentNetworkMode;
TelloConnectWidget::TelloConnectWidget(NetworkSettingsWidget *nsw, Device *device, QWidget *parent)
    : StandardWidget(device, parent), ui(new Ui::TelloConnectWidget), nsw(nsw)
{
    ui->setupUi(this);
    performStandardSetup(tr("Network Settings"));

    enableCoolOffTimer = new QTimer(this);
    enableCoolOffTimer->setSingleShot(true);

    // ui->ConnectButton->setEnabled(false);
    QObject::connect(ui->ConnectButton, SIGNAL(clicked()), SLOT(connectToButton()));
    QObject::connect(ui->ManageButton, SIGNAL(clicked()), SLOT(manage()));

    // QObject::connect(ui->clientModeButton, SIGNAL(clicked()), this, &NetworkSettingsWidget::connect());
    QObject::connect(ui->clientModeButton, SIGNAL(clicked()), this, SLOT(enableClientMode()));

    // TODO: put back after we support client mode WiFi
    ui->ConnectButton->setVisible(true);
    ui->ManageButton->setVisible(true);
    ui->security->setVisible(false);
    ui->securityLabel->setVisible(false);

    QObject::connect(&NetworkManager::ref(),
                     SIGNAL(stateChanged(const NetworkManager::State &, const NetworkManager::State &)),
                     SLOT(stateChanged(const NetworkManager::State &, const NetworkManager::State &)));

    currentNetworkMode = getCurrentConnectionMode();

    ui->currentModeLabel->setText(currentNetworkMode);

    if (currentNetworkMode == "AP Mode")
    {
        ui->ConnectButton->setEnabled(false);
        ui->ManageButton->setEnabled(false);
        ui->clientModeButton->setEnabled(true);
    }
    else if (currentNetworkMode == "Client Mode")
    {
        ui->clientModeButton->setEnabled(false);
    }

    QTimer *updateTimer = new QTimer(this);
    QObject::connect(updateTimer, SIGNAL(timeout()), SLOT(updateInformation()));
    updateTimer->start(10000);

    updateInformation();
}

QString TelloConnectWidget::getCurrentConnectionMode()
{
    // Assume controller isn't in client mode already
    QString value = nsw->sendCurrentConnectionMode();
    qDebug() << "ComboBox Value:" << value;

    return value;
}

void TelloConnectWidget::enableClientMode()
{

    // If current network mode is AP
    if (NetworkManager::ref().currentActiveConnectionName() == NetworkManager::ref().getAPName())
    {
        NetworkManager::ref().disableAP();
    }
    ui->currentModeLabel->setText("Client Mode");
    ui->ConnectButton->setEnabled(true);
    ui->ManageButton->setEnabled(true);
}

void TelloConnectWidget::connectToButton()
{
    RootController::ref().presentWidget(new TelloNetworksWidget(device()));
}

void TelloConnectWidget::indexChanged(int index)
{
    NetworkManager::ref().turnOn();

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

TelloConnectWidget::~TelloConnectWidget()
{
    delete ui;
    delete enableCoolOffTimer;
}

void TelloConnectWidget::manage() // Forget or add network to history
{
    RootController::ref().presentWidget(new ManageNetworksWidget(device()));
}

void TelloConnectWidget::updateInformation()
{
    const bool on = NetworkManager::ref().isOn(); //
    ui->ssid->setText(NetworkManager::ref().currentActiveConnectionName());
    ui->ip->setText(NetworkManager::ref().ip4Address());

    const QString id = device()->id();
    const QString serial = device()->serial();

    if (!on) // wifi is off
    {
        // ui->clientModeButton->setCurrentText("Wifi Off");
        ui->state->setText("OFF");
        ui->ssid->setText(" ");
        ui->ip->setText(" ");
        ui->password->setText(" ");
    }
    else if (NetworkManager::ref().isActiveConnectionOn() == true) // if there's an active connection
    {

        if (NetworkManager::ref().currentActiveConnectionName() != NetworkManager::ref().getAPName()) // if current mode isn't AP
        {
            // ui->clientModeButton->setCurrentIndex(1);
            ui->state->setText(on ? tr("ON") : tr("OFF"));
            ui->ssid->setText(NetworkManager::ref().currentActiveConnectionName());
            ui->ip->setText(NetworkManager::ref().ip4Address());
        }
        else // if current mode is AP
        {

            ui->state->setText(on ? tr("ON") : tr("OFF"));
            ui->ssid->setText(NetworkManager::ref().currentActiveConnectionName());
            ui->ip->setText(NetworkManager::ref().ip4Address());
        }
        ui->password->setText(NetworkManager::ref().activeConnectionPassword());
    }

    Network active = NetworkManager::ref().active();

    ui->security->setText(active.securityString());
}

void TelloConnectWidget::stateChanged(const NetworkManager::State &newState, const NetworkManager::State &oldState)
{
    qDebug() << "State Changed to" << newState;
    QTimer::singleShot(300, this, SLOT(updateInformation()));
}

#endif
