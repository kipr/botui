#include "ServosWidget.h"
#include "ui_ServosWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include <QDebug>

ServosWidget::ServosWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::ServosWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Servos"));
}

ServosWidget::~ServosWidget()
{
	delete ui;
}
