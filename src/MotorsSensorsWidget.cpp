#include "MotorsSensorsWidget.h"
#include "ui_MotorsSensorsWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include <QDebug>
#include "CameraWidget.h"

MotorsSensorsWidget::MotorsSensorsWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::MotorsSensorsWidget),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this))
{
	ui->setupUi(this);
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle("Motors and Sensors");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
	
	connect(ui->camera, SIGNAL(clicked()), SLOT(camera()));
}

MotorsSensorsWidget::~MotorsSensorsWidget()
{
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}

void MotorsSensorsWidget::camera()
{
	RootController::ref().presentWidget(new CameraWidget(m_device));
}