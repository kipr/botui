#include "CompilingWidget.h"
#include "Device.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "ui_CompilingWidget.h"
#include <QDebug>

CompilingWidget::CompilingWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent), ui(new Ui::CompilingWidget) {
    ui->setupUi(this);
    performStandardSetup(tr("Compiling..."));
}

CompilingWidget::~CompilingWidget() { delete ui; }

void CompilingWidget::compileFinished() {}
