#include "ServosWidget.h"
#include "ui_ServosWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "NumpadDialog.h"

#ifdef WALLABY
#include "wallaby/wallaby.h"
#else
#include "kovan/kovan.h"
#endif

#include <QDebug>

ServosWidget::ServosWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::ServosWidget),
	m_provider(new NumpadDialog(QString()))
{
	ui->setupUi(this);
	performStandardSetup(tr("Servos"));
	
	ui->number->setInputProvider(m_provider);
	
	connect(ui->dial, SIGNAL(valueChanged(double)), SLOT(valueChanged(double)));
	connect(ui->_0, SIGNAL(clicked()), SLOT(activeChanged()));
	connect(ui->_1, SIGNAL(clicked()), SLOT(activeChanged()));
	connect(ui->_2, SIGNAL(clicked()), SLOT(activeChanged()));
	connect(ui->_3, SIGNAL(clicked()), SLOT(activeChanged()));
	connect(ui->number, SIGNAL(textEdited(QString)), SLOT(manualEntry(QString)));
	connect(ui->enable, SIGNAL(clicked()), SLOT(enable()));
	connect(ui->disable, SIGNAL(clicked()), SLOT(disable()));
	
	ui->dial->setMinimumValue(0);
	ui->dial->setMaximumValue(2047);
	ui->dial->setValue(1024);
	
	ui->dial->setLabel(0);
	ui->_0->setEnabled(false);
	
	activeChanged();

	publish();
}

ServosWidget::~ServosWidget()
{
	disable_servos();
	publish();
	delete ui;
	delete m_provider;
}

void ServosWidget::valueChanged(const double &value)
{
	ui->number->setText(QString::number((int)ui->dial->value()));
	double safeValue = value;
	if(safeValue < 50.0) safeValue = 50.0;
	else if(safeValue > 1998.0) safeValue = 1998.0;
	
	set_servo_position(ui->dial->label(), safeValue);
	publish();
}

void ServosWidget::activeChanged()
{
	QObject *from = sender();
	if(!from) from = ui->_0;
	
	disable_servo(ui->dial->label());
	
	quint16 label = 0xFFFF;
	
	if(from == ui->_0) label = 0;
	else if(from == ui->_1) label = 1;
	else if(from == ui->_2) label = 2;
	else if(from == ui->_3) label = 3;
	
	ui->_0->setEnabled(from != ui->_0);
	ui->_1->setEnabled(from != ui->_1);
	ui->_2->setEnabled(from != ui->_2);
	ui->_3->setEnabled(from != ui->_3);
	
	ui->enable->setVisible(!get_servo_enabled(label));
	ui->disable->setVisible(get_servo_enabled(label));
	
	ui->dial->setLabel(label);
	publish();
	ui->dial->setValue(get_servo_position(label));
}

void ServosWidget::enable()
{
	enable_servo(ui->dial->label());
	publish();
	ui->enable->hide();
	ui->disable->show();
}

void ServosWidget::disable()
{
	disable_servo(ui->dial->label());
	publish();
	ui->enable->show();
	ui->disable->hide();
}

void ServosWidget::manualEntry(const QString &text)
{
	ui->dial->setValue(text.toInt());
}
