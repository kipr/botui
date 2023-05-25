#include "Options.h"
#include <QCoreApplication>
#ifdef NETWORK_ENABLED

#include "ConnectWidget.h"
#include "Device.h"
#include "Execute.h"
#include "ManageNetworksWidget.h"
#include "MenuBar.h"
#include "NetworkManager.h"
#include "NetworkSettingsWidget.h"
#include "NetworkStatusWidget.h"
#include "RootController.h"
#include "StatusBar.h"
#include "SystemUtils.h"
#include "ui_NetworkSettingsWidget.h"

#include <QTimer>

#include <QDebug>

NetworkSettingsWidget::NetworkSettingsWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent), ui(new Ui::NetworkSettingsWidget) {
    ui->setupUi(this);
    performStandardSetup(tr("Network Settings"));

    enableCoolOffTimer = new QTimer(this);
    enableCoolOffTimer->setSingleShot(true);
    QObject::connect(
        enableCoolOffTimer, SIGNAL(timeout()), SLOT(enableAPControls()));

    ui->ConnectButton->setEnabled(false);
    QObject::connect(ui->ConnectButton, SIGNAL(clicked()), SLOT(connect()));
    QObject::connect(ui->ManageButton, SIGNAL(clicked()), SLOT(manage()));

    QObject::connect(
        ui->TournamentModeButton, SIGNAL(clicked()), SLOT(TournamentMode()));
    NetworkManager::ref().connect(ui->connectionModeSelect,
                                  SIGNAL(currentIndexChanged(int)),
                                  this,
                                  SLOT(indexChanged(int)));

    // TODO: put back after we support client mode WiFi
    ui->ConnectButton->setVisible(true);
    ui->ManageButton->setVisible(true);
    ui->security->setVisible(false);
    ui->securityLabel->setVisible(false);

    QObject::connect(&NetworkManager::ref(),
                     SIGNAL(stateChanged(const NetworkManager::State &,
                                         const NetworkManager::State &)),
                     SLOT(stateChanged(const NetworkManager::State &,
                                       const NetworkManager::State &)));

    QTimer *updateTimer = new QTimer(this);
    QObject::connect(updateTimer, SIGNAL(timeout()), SLOT(updateInformation()));
    updateTimer->start(10000);

    setupConnectionModeSelect();
    updateInformation();
}

void NetworkSettingsWidget::setupConnectionModeSelect() {
    firstTimeSetup = true;
    const bool on = NetworkManager::ref().isOn();
    if (!on) {
        ui->ConnectButton->setEnabled(false);
        ui->connectionModeSelect->setCurrentIndex(3); // no wifi
    }
    else if (NetworkManager::ref().isActiveConnectionOn()) {
        if (NetworkManager::ref().isActiveConnectionAP()) {
            ui->ConnectButton->setEnabled(false);
            ui->connectionModeSelect->setCurrentIndex(1); // ap mode
        }
        else {
            ui->ConnectButton->setEnabled(true);
            ui->connectionModeSelect->setCurrentIndex(
                2); // some other client mode
        }
    }
    else {
        // case where wifi is on but it isn't connected to anything yet
        // happens in client mode
        ui->ConnectButton->setEnabled(true);
        ui->connectionModeSelect->setCurrentIndex(2);
    }

    firstTimeSetup = false;
}

void NetworkSettingsWidget::TournamentMode() {
    ui->connectionModeSelect->setCurrentIndex(
        3); // tournament mode = turn off wifi
}

void NetworkSettingsWidget::indexChanged(int index) {

    if (firstTimeSetup) return;
    qDebug() << "going to change stuff in index changed";
    if (index == 1) // Wifi on (AP mode)
    {
        ui->ConnectButton->setEnabled(false);
        NetworkManager::ref()
            .turnOn(); // turn on before enabling AP in case wifi was off before
        NetworkManager::ref().enableAP();
    }
    else if (index == 2) // Wifi on (client mode)
    {
        NetworkManager::ref().disableAP();
        ui->ConnectButton->setEnabled(true);
    }
    else if (index == 3) // Wifi off
    {
        NetworkManager::ref().turnOff();
        ui->ConnectButton->setEnabled(false);
    }
}

NetworkSettingsWidget::~NetworkSettingsWidget() {
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

void NetworkSettingsWidget::updateInformation() {
    // clear old values
    ui->ssid->setText("");
    ui->ip->setText("");
    ui->security->setText("");
    ui->password->setText("");

    if (NetworkManager::ref()
            .isActiveConnectionOn()) // if there's an active connection
    {
        Network active = NetworkManager::ref().active();
        ui->ssid->setText(active.ssid());
        ui->ip->setText(NetworkManager::ref().ip4Address());
        ui->security->setText(active.securityString());

        QString password = NetworkManager::ref().activeConnectionPassword();
        ui->password->setText(password);
    }
}

void NetworkSettingsWidget::stateChanged(
    const NetworkManager::State &newState,
    const NetworkManager::State &oldState) {
    qDebug() << "State Changed to" << newState;
    QTimer::singleShot(300, this, SLOT(updateInformation()));
}

#endif
