#include "MotorsSensorsWidget.h"
#include "ui_MotorsSensorsWidget.h"
#include "RootController.h"
#include <QDebug>
#include "CameraWidget.h"
#include "ServosWidget.h"
#include "MotorsWidget.h"
#include "PidWidget.h"
#include "PidTunerWidget.h"
#include "SensorsWidget.h"
#include "NotYetImplementedDialog.h"

MotorsSensorsWidget::MotorsSensorsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::MotorsSensorsWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Motors and Sensors"));
	
	connect(ui->servos, SIGNAL(clicked()), SLOT(servos()));
	connect(ui->motors, SIGNAL(clicked()), SLOT(motors()));
	connect(ui->pid, SIGNAL(clicked()), SLOT(pid()));
	connect(ui->sensors, SIGNAL(clicked()), SLOT(sensors()));
	connect(ui->camera, SIGNAL(clicked()), SLOT(camera()));
	connect(ui->pidTuner, SIGNAL(clicked()), SLOT(pidTuner()));
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
	RootController::ref().presentWidget(new MotorsWidget(device()));
}

void MotorsSensorsWidget::pid()
{
	RootController::ref().presentWidget(new PidWidget(device()));
}

void MotorsSensorsWidget::sensors()
{
	RootController::ref().presentWidget(new SensorsWidget(device()));
}

void MotorsSensorsWidget::camera()
{
	RootController::ref().presentWidget(new CameraWidget(device()));
}

void MotorsSensorsWidget::pidTuner()
{
	RootController::ref().presentWidget(new PidTunerWidget(device()));
}
