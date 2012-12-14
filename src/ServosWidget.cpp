#include "ServosWidget.h"
#include "ui_ServosWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "kovan/kovan.h"
#include <QDebug>

ServosWidget::ServosWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::ServosWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Servos"));
	
	connect(ui->dial, SIGNAL(valueChanged(double)), SLOT(valueChanged(double)));
	connect(ui->_0, SIGNAL(clicked()), SLOT(activeChanged()));
	connect(ui->_1, SIGNAL(clicked()), SLOT(activeChanged()));
	connect(ui->_2, SIGNAL(clicked()), SLOT(activeChanged()));
	connect(ui->_3, SIGNAL(clicked()), SLOT(activeChanged()));
	
	ui->dial->setMinimumValue(0);
	ui->dial->setMinimumValue(1024);
	ui->dial->setValue(512);
	ui->dial->setLabel(0);
}

ServosWidget::~ServosWidget()
{
	delete ui;
}

void ServosWidget::valueChanged(const double &value)
{
	set_servo_position(ui->dial->label(), value);
}

void ServosWidget::activeChanged()
{
	QObject *from = sender();
	if(!from) return;
	
	quint16 label = 0xFFFF;
	
	if(from == ui->_0) label = 0;
	else if(from == ui->_1) label = 1;
	else if(from == ui->_2) label = 2;
	else if(from == ui->_3) label = 3;
	
	ui->_0->setEnabled(from != ui->_0);
	ui->_1->setEnabled(from != ui->_1);
	ui->_2->setEnabled(from != ui->_3);
	ui->_3->setEnabled(from != ui->_3);
	
	ui->dial->setLabel(label);
	ui->dial->setValue(512);
	
}