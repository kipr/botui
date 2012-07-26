#include "PackageLandingWidget.h"
#include "ui_PackageLandingWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include <QDebug>

PackageLandingWidget::PackageLandingWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::PackageLandingWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Packages"));
}

PackageLandingWidget::~PackageLandingWidget()
{
	delete ui;
}
