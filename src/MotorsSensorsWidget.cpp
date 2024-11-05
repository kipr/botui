#include "MotorsSensorsWidget.h"
#include "ui_MotorsSensorsWidget.h"
#include "RootController.h"
#include <QDebug>
#include "CameraLiveWidget.h"
#include "ServosWidget.h"
#include "CombinedMotorWidget.h"
#include "DepthSensorWidget.h"
#include "SensorsWidget.h"
#include "SensorListWidget.h"
#include "NotYetImplementedDialog.h"

MotorsSensorsWidget::MotorsSensorsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::MotorsSensorsWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Motors and Sensors"));
	
	connect(ui->servos, SIGNAL(clicked()), SLOT(servos()));
	connect(ui->motors, SIGNAL(clicked()), SLOT(motors()));
	connect(ui->sensorGraph, SIGNAL(clicked()), SLOT(sensorGraph()));
	connect(ui->sensorList, SIGNAL(clicked()), SLOT(sensorList()));
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

void MotorsSensorsWidget::motors()
{
	RootController::ref().presentWidget(new CombinedMotorWidget(device()));
}

void MotorsSensorsWidget::sensorGraph()
{
	RootController::ref().presentWidget(new SensorsWidget(device()));
}

void MotorsSensorsWidget::sensorList()
{
	RootController::ref().presentWidget(new SensorListWidget(device()));
}

void MotorsSensorsWidget::camera()
{
	RootController::ref().presentWidget(new CameraLiveWidget(device()));
}


