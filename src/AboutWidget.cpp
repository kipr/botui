#include "AboutWidget.h"
#include "ui_AboutWidget.h"
#include "RootController.h"
#include "Device.h"

#include <QDebug>

AboutWidget::AboutWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::AboutWidget)
{
	ui->setupUi(this);
	ui->deviceName->setText(device->name() + " v" + device->version());
	performStandardSetup(tr("About"));
}

AboutWidget::~AboutWidget()
{
	delete ui;
}
