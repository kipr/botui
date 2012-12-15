#include "MotorsSensorsWidget.h"
#include "ui_MotorsSensorsWidget.h"
#include "RootController.h"
#include <QDebug>
#include "CameraWidget.h"
#include "ServosWidget.h"
#include "SensorsWidget.h"
#include "NotYetImplementedDialog.h"

MotorsSensorsWidget::MotorsSensorsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::MotorsSensorsWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Motors and Sensors"));
	
	connect(ui->servos, SIGNAL(clicked()), SLOT(servos()));
	connect(ui->sensors, SIGNAL(clicked()), SLOT(sensors()));
	connect(ui->camera, SIGNAL(clicked()), SLOT(camera()));
}

MotorsSensorsWidget::~MotorsSensorsWidget()
{
	delete ui;
}

void MotorsSensorsWidget::servos()
{
	RootController::ref().presentWidget(new ServosWidget(device()));
}

void MotorsSensorsWidget::sensors()
{
	RootController::ref().presentWidget(new SensorsWidget(device()));
}

void MotorsSensorsWidget::camera()
{
	RootController::ref().presentWidget(new CameraWidget(device()));
}