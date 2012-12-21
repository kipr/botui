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
	
	const double val1 = value(ui->plot1->currentIndex());
	const double val2 = value(ui->plot2->currentIndex());
	
	ui->val1->setText(QString::number(val1));
	ui->val2->setText(QString::number(val2));
	
	ui->plot->push(m_plots[0], val1);
	ui->plot->push(m_plots[1], val2);
	ui->plot->inc();
}

double SensorsWidget::value(const int &i)
{
	double val = 0.0;
	if(i < 8) val = analog10(i) / 512.0 - 1.0;
	else if(i < 12) val = get_backemf(i - 8) / 32768.0;
	else if(i == 12) val = accel_x() / 127.5 - 1.0;
	else if(i == 13) val = accel_y() / 127.5 - 1.0;
	else if(i == 14) val = accel_z() / 127.5 - 1.0;
	return val;
}