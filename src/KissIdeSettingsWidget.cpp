#include "KissIdeSettingsWidget.h"
#include "Device.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "ui_KissIdeSettingsWidget.h"
#include <QDebug>

KissIdeSettingsWidget::KissIdeSettingsWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent), ui(new Ui::KissIdeSettingsWidget) {
    ui->setupUi(this);
    performStandardSetup(tr("KISS IDE Settings"));
}

KissIdeSettingsWidget::~KissIdeSettingsWidget() { delete ui; }
