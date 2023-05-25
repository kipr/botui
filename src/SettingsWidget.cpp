#include "SettingsWidget.h"
#include "Calibrate.h"
#include "Device.h"
#include "MenuBar.h"
#include "NotYetImplementedDialog.h"
#include "Options.h"
#include "RootController.h"
#include "SettingsProvider.h"
#include "StatusBar.h"
#include "ui_SettingsWidget.h"

#include "AdvancedSettingsWidget.h"
#include "BackupWidget.h"
#include "CameraSettingsWidget.h"
#include "ChannelConfigurationsWidget.h"
#include "LanguageWidget.h"
#include "WombatUpdateWidget.h"

#include <QDebug>

SettingsWidget::SettingsWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent), ui(new Ui::SettingsWidget) {
    ui->setupUi(this);
    performStandardSetup(tr("Settings"));

    const SettingsProvider *const settingsProvider = device->settingsProvider();
    if (settingsProvider) {
        const bool hideUI = settingsProvider->value("hideUI").toBool();
        ui->hideUi->setVisible(hideUI);
    }

    connect(ui->channels, SIGNAL(clicked()), SLOT(channels()));
    connect(ui->calibrate, SIGNAL(clicked()), SLOT(calibrate()));
    connect(ui->language, SIGNAL(clicked()), SLOT(language()));
    connect(ui->update, SIGNAL(clicked()), SLOT(update()));
    connect(ui->hideUi, SIGNAL(clicked()), SLOT(hideUi()));
    connect(ui->cameraView, SIGNAL(clicked()), SLOT(cameraView()));
    connect(ui->backup, SIGNAL(clicked()), SLOT(backup()));
    connect(ui->advanced, SIGNAL(clicked()), SLOT(advanced()));
}

SettingsWidget::~SettingsWidget() { delete ui; }

void SettingsWidget::advanced() {
    RootController::ref().presentWidget(new AdvancedSettingsWidget(device()));
}

void SettingsWidget::channels() {
    RootController::ref().presentWidget(
        new ChannelConfigurationsWidget(device()));
}

void SettingsWidget::calibrate() { Calibrate::calibrate(); }

void SettingsWidget::language() {
    RootController::ref().presentWidget(new LanguageWidget(device()));
}

void SettingsWidget::update() {
    WombatUpdateWidget *const updateWidget = new WombatUpdateWidget(device());
    RootController::ref().presentWidget(updateWidget);
    updateWidget->refresh();
}

void SettingsWidget::hideUi() { RootController::ref().minimize(); }

void SettingsWidget::cameraView() {
    RootController::ref().presentWidget(new CameraSettingsWidget(device()));
}
void SettingsWidget::backup() {
    RootController::ref().presentWidget(new BackupWidget(device()));
}
