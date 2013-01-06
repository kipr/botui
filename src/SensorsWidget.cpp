#include "SensorsWidget.h"
#include "ui_SensorsWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "NetworkManager.h"
#include "NetworkItemModel.h"
#include "OtherNetworkWidget.h"
#include "Device.h"
#include <QTimer>

#include <kovan/kovan.h>

#include <QDebug>

#include <math.h> // Temporary

SensorsWidget::SensorsWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	m_menuBar(new MenuBar(this)),
	ui(new Ui::SensorsWidget)
{
	ui->setupUi(this);
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle("Sensors");
	layout()->setMenuBar(m_menuBar);
	
	m_plots[0] = ui->plot->addPlot(QColor(200, 0, 0));
	m_plots[1] = ui->plot->addPlot(QColor(0, 0, 200));
	
	QTimer *updateTimer = new QTimer(this);
	connect(updateTimer, SIGNAL(timeout()), SLOT(update()));
	updateTimer->start(10);
}

SensorsWidget::~SensorsWidget()
{
	delete ui;
}

void SensorsWidget::update()
{
	publish();
	
	ui->val1->setText(QString::number(rawValue(ui->plot1->currentIndex())));
	ui->val2->setText(QString::number(rawValue(ui->plot2->currentIndex())));
	
	ui->plot->push(m_plots[0], value(ui->plot1->currentIndex()));
	ui->plot->push(m_plots[1], value(ui->plot2->currentIndex()));
	ui->plot->inc();
}

double SensorsWidget::rawValue(const int &i) const
{
	double val = 0;
	if(i < 8) val = analog10(i);
	else if(i < 12) val = get_motor_position_counter(i - 8);
	else if(i == 12) val = accel_x();
	else if(i == 13) val = accel_y();
	else if(i == 14) val = accel_z();
	return val;
}

double SensorsWidget::value(const int &i) const
{
	double val = rawValue(i);
	
	if(i < 8) val = val / 512.0 - 1.0;
	else if(i < 12) val = val / 32768.0;
	else if(i < 15) val = val / 127.5 - 1.0;
	else val = 0.0;
	
	return val;
}