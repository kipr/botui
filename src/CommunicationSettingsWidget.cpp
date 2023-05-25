#include "CommunicationSettingsWidget.h"
#include "ui_CommunicationSettingsWidget.h"

#include "KeyboardDialog.h"
#include "MenuBar.h"
#include "SystemUtils.h"

#include <kipr/config/config.hpp>
using namespace kipr::config;

#include <QDebug>
#include <QString>

#define SETTINGS_FILE "/etc/kovan/device.conf"
#define DEVICE_NAME_KEY "device_name"
#define KOVAN_SERIAL_GROUP "kovan_serial"
#define PASSWORD_KEY "password"

CommunicationSettingsWidget::CommunicationSettingsWidget(Device *device,
                                                         QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::CommunicationSettingsWidget()) {
    ui->setupUi(this);
    performStandardSetup(tr("Communication Settings"), false);
    ui->deviceName->setInputProvider(
        new KeyboardDialog(tr("Device Name"), KeyboardDialog::Normal, this));
    ui->password->setInputProvider(
        new KeyboardDialog(tr("Password"), KeyboardDialog::Normal, this));

    connect(ui->deviceName,
            SIGNAL(textChanged(QString)),
            SLOT(deviceNameChanged(QString)));
    connect(ui->passworded,
            SIGNAL(stateChanged(int)),
            SLOT(passwordedChanged(int)));
    connect(ui->password,
            SIGNAL(textChanged(QString)),
            SLOT(passwordChanged(QString)));

    Config *settings = Config::load(SETTINGS_FILE);
    QString password = "";
    if (settings) {
        QString name =
            QString::fromStdString(settings->stringValue(DEVICE_NAME_KEY));
        ui->deviceName->setText(name);
        settings->beginGroup(KOVAN_SERIAL_GROUP);
        password = QString::fromStdString(settings->stringValue(PASSWORD_KEY));
        ui->passworded->setCheckState(password.isEmpty() ? Qt::Unchecked
                                                         : Qt::Checked);
        settings->endGroup();
    }
    delete settings;

    passwordedChanged(ui->passworded->checkState());
    ui->password->setText(password);
}

CommunicationSettingsWidget::~CommunicationSettingsWidget() {}

void CommunicationSettingsWidget::deviceNameChanged(const QString &text) {
    Config *settings = Config::load(SETTINGS_FILE);
    if (!settings) settings = new Config();
    settings->setValue(DEVICE_NAME_KEY, ui->deviceName->text().toStdString());
    // TODO: Error checking?
    settings->save(SETTINGS_FILE);
    delete settings;
}

void CommunicationSettingsWidget::passwordedChanged(const int state) {
    const bool enable = state == Qt::Checked;
    // Force update
    ui->password->setText("");
    passwordChanged(ui->password->text());
    ui->password->setEnabled(enable);
}

void CommunicationSettingsWidget::passwordChanged(const QString &text) {
    const bool enabled = ui->passworded->checkState() == Qt::Checked;
    const bool good = text.size() >= 3 && text.size() <= 10;
    ui->passBad->setVisible(!good && enabled);
    ui->passGood->setVisible(good && enabled);
    menuBar()->setEnabled(good || !enabled);

    if (!good && enabled) return;

    Config *settings = Config::load(SETTINGS_FILE);
    if (!settings) settings = new Config();
    settings->beginGroup(KOVAN_SERIAL_GROUP);
    settings->setValue(PASSWORD_KEY, text.toStdString());
    settings->endGroup();

    // TODO: Error checking?
    settings->save(SETTINGS_FILE);
    if (!SystemUtils::setUserPassword(text)) {
        qWarning() << "Failed to update system password";
    }

    delete settings;
}
