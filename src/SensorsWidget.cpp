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
	m_hideOptions("Hide Options", this),
	ui(new Ui::SensorsWidget)
{
	ui->setupUi(this);
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle("Sensors");
	m_menuBar->addAction(&m_hideOptions);
	layout()->setMenuBar(m_menuBar);
	
	m_plots[0] = ui->plot->addPlot(QColor(0, 0, 200));
	m_plots[1] = ui->plot->addPlot(QColor(200, 0, 0));
	
	connect(&m_hideOptions, SIGNAL(activated()), SLOT(toggleUi()));
	
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
	static double value = 0.0;
	
	publish();
	ui->plot->push(m_plots[0], analog10(ui->plot1->currentIndex()) / 512.0 - 1.0);
	ui->plot->push(m_plots[1], analog10(ui->plot2->currentIndex()) / 512.0 - 1.0);
	ui->plot->inc();
	
	value += 0.1;
}

void SensorsWidget::toggleUi()
{
	ui->options->setVisible(!ui->options->isVisible());
	m_hideOptions.setText(ui->options->isVisible() ? "Hide Options" : "Show Options");
}