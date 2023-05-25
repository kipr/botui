#include "BatterySettingsWidget.h"
#include "ui_BatterySettingsWidget.h"

#include <QDebug>
#include <QMessageBox>

#include "Device.h"
#include "SettingsProvider.h"

#define BATTERY_TYPE_KEY "battery_type"
#define BATTERY_WARNING_ENABLED "battery_warning_enabled"
#define BATTERY_WARNING_THRESH "battery_warning_thresh"

BatterySettingsWidget::BatterySettingsWidget(Device *device,
                                             QWidget *const parent)
    : StandardWidget(device, parent),
      ui(new Ui::BatterySettingsWidget),
      m_numpadInput(new NumpadDialog(
          "Warning Threshold", NumpadDialog::Integer, 0, 100, this)) {
    ui->setupUi(this);
    performStandardSetup(tr("Battery Settings Widget"));

    // TODO: Populate combo box with availalbe battery types

    const SettingsProvider *const settingsProvider = device->settingsProvider();
    if (settingsProvider) {
        // Set current battery type
        const int currType =
            settingsProvider->value(BATTERY_TYPE_KEY, 0).toInt();
        if (currType >= 0 && currType <= 2) {
            m_currType = currType;
            ui->batteryType->setCurrentIndex(currType);
        }

        // Set current warning settings
        const bool warnEnabled =
            settingsProvider->value(BATTERY_WARNING_ENABLED, true).toBool();
        const float warnThresh =
            settingsProvider->value(BATTERY_WARNING_THRESH, 0.1f).toFloat();
        const int warnThreshPct = (int)(warnThresh * 100 + 0.5f);
        ui->warningEnabled->setChecked(warnEnabled);
        ui->warningThresh->setText(QString::number(warnThreshPct));
        ui->warningThresh->setVisible(warnEnabled);
        ui->pctLabel->setVisible(warnEnabled);
    }

    ui->warningThresh->setInputProvider(m_numpadInput);

    connect(ui->batteryType,
            SIGNAL(currentIndexChanged(int)),
            SLOT(typeChanged(int)));
    connect(ui->warningEnabled,
            SIGNAL(stateChanged(int)),
            SLOT(warnEnabledChanged(int)));
    connect(ui->warningThresh,
            SIGNAL(textEdited(QString)),
            SLOT(warnThreshChanged(QString)));
}

BatterySettingsWidget::~BatterySettingsWidget() {
    delete m_numpadInput;
    delete ui;
}

void BatterySettingsWidget::typeChanged(int type) {
    // Do nothing if we're changing to the current type
    if (type == m_currType) return;

    SettingsProvider *const settingsProvider = device()->settingsProvider();
    if (!settingsProvider) return;

    // Make sure user ACTUALLY wants to change the type
    const QString typeName = ui->batteryType->currentText();
    if (QMessageBox::question(
            this,
            "Change battery type?",
            QString("Are you sure you have a %1 battery?").arg(typeName),
            QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
    {
        ui->batteryType->setCurrentIndex(m_currType);
        return;
    }

    m_currType = type;
    settingsProvider->setValue(BATTERY_TYPE_KEY, type);
    settingsProvider->sync();
}

void BatterySettingsWidget::warnEnabledChanged(int state) {
    SettingsProvider *const settingsProvider = device()->settingsProvider();
    if (!settingsProvider) return;

    const bool enabled = state;
    ui->pctLabel->setVisible(enabled);
    ui->warningThresh->setVisible(enabled);
    settingsProvider->setValue(BATTERY_WARNING_ENABLED, enabled);
    settingsProvider->sync();
}

void BatterySettingsWidget::warnThreshChanged(QString text) {
    SettingsProvider *const settingsProvider = device()->settingsProvider();
    if (!settingsProvider) return;

    const int warnThreshPct = text.toInt();
    const float warnThresh = warnThreshPct / 100.0;

    settingsProvider->setValue(BATTERY_WARNING_THRESH, warnThresh);
    settingsProvider->sync();
}