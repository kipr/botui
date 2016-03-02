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
#include <math.h>

ServosWidget::ServosWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::ServosWidget),
	m_provider(new NumpadDialog(QString()))
{
	ui->setupUi(this);
	performStandardSetup(tr("Servos"));
	
	ui->number->setInputProvider(m_provider);
	
	connect(ui->dial, SIGNAL(valueChanged(double)), SLOT(positionChanged(double)));
  connect(ui->number, SIGNAL(textEdited(QString)), SLOT(manualEntry(QString)));
	connect(ui->port_0, SIGNAL(clicked()), SLOT(activeChanged()));
	connect(ui->port_1, SIGNAL(clicked()), SLOT(activeChanged()));
	connect(ui->port_2, SIGNAL(clicked()), SLOT(activeChanged()));
	connect(ui->port_3, SIGNAL(clicked()), SLOT(activeChanged()));
	connect(ui->enable, SIGNAL(clicked()), SLOT(enable()));
	connect(ui->disable, SIGNAL(clicked()), SLOT(disable()));
	
  // Setup ui for port 0 (the default servo)
	ui->dial->setMinimumValue(0);
	ui->dial->setMaximumValue(2047);
	ui->dial->setValue(get_servo_position(0));
	ui->dial->setLabel(0);
	ui->port_0->setEnabled(false);
  
  const bool servoEnabled = get_servo_enabled(0);
	ui->enable->setVisible(!servoEnabled);
	ui->disable->setVisible(servoEnabled);
  
	publish();
}

ServosWidget::~ServosWidget()
{
	delete ui;
	delete m_provider;
}

void ServosWidget::positionChanged(const double &value)
{
  // Keep position label in sync with dial
	ui->number->setText(QString::number(round(value)));
  
	set_servo_position(ui->dial->label(), round(value));
  
	publish();
}

void ServosWidget::activeChanged()
{
	QObject *from = sender();
		
  // Set dial label for new servo
  int newPort = -1;
	if(from == ui->port_0) newPort = 0;
	else if(from == ui->port_1) newPort = 1;
	else if(from == ui->port_2) newPort = 2;
	else if(from == ui->port_3) newPort = 3;
  else return;
  ui->dial->setLabel(newPort);
  
  // Set dial value for new servo
  ui->dial->setValue(get_servo_position(newPort));
	
  // Enable/disable appropriate port buttons
	ui->port_0->setEnabled(from != ui->port_0);
	ui->port_1->setEnabled(from != ui->port_1);
	ui->port_2->setEnabled(from != ui->port_2);
	ui->port_3->setEnabled(from != ui->port_3);
	
  // Show/hide enable and disable buttons
  const bool servoEnabled = get_servo_enabled(newPort);
	ui->enable->setVisible(!servoEnabled);
	ui->disable->setVisible(servoEnabled);
	
	publish();
}

void ServosWidget::enable()
{
  // Enable current servo
  const int port = ui->dial->label();
	enable_servo(port);
	publish();
  
  // Update enable/disable button visibility
	ui->enable->hide();
	ui->disable->show();
}

void ServosWidget::disable()
{
  // Disable current servo
  const int port = ui->dial->label();
	disable_servo(port);
	publish();
  
  // Update enable/disable button visibility
	ui->enable->show();
	ui->disable->hide();
}

void ServosWidget::manualEntry(const QString &text)
{
  // Keep dial in sync with position label
	ui->dial->setValue(text.toInt());
}
