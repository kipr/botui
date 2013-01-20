#include "PidTunerWidget.h"
#include "ui_PidTunerWidget.h"

#include "NumpadDialog.h"
#include <QDebug>

#include <kovan/motors.h>
#include <kovan/general.h>
#include <cmath>

#define MOTOR_SCALING (1500.0)

PidTunerWidget::PidTunerWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::PidTunerWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("PID Tuner"), false);
	connect(ui->p, SIGNAL(textChanged(QString)), SLOT(coeffChanged()));
	connect(ui->i, SIGNAL(textChanged(QString)), SLOT(coeffChanged()));
	connect(ui->d, SIGNAL(textChanged(QString)), SLOT(coeffChanged()));
	connect(ui->motor, SIGNAL(currentIndexChanged(int)), SLOT(motorChanged()));

	NumpadDialog *pProvider = new NumpadDialog(tr("P Value"), NumpadDialog::Decimal, -99.0, 99.0, this);
	NumpadDialog *iProvider = new NumpadDialog(tr("I Value"), NumpadDialog::Decimal, -99.0, 99.0, this);
	NumpadDialog *dProvider = new NumpadDialog(tr("D Value"), NumpadDialog::Decimal, -99.0, 99.0, this);

	ui->p->setInputProvider(pProvider);
	ui->i->setInputProvider(iProvider);
	ui->d->setInputProvider(dProvider);


	m_feedback = ui->plot->addPlot(QColor(0, 0, 200));
	m_setpoint = ui->plot->addPlot(QColor(0, 0, 0));


	ui->p->setText("2.0");
	ui->i->setText("0.0");
	ui->d->setText("0.0");


	QTimer *updateTimer = new QTimer(this);
	connect(updateTimer, SIGNAL(timeout()), SLOT(update()));
	updateTimer->start(25);

	connect(ui->plot, SIGNAL(mouseEvent(double)), SLOT(mouseEvent(double)));


	m_setpointVal = 0.0;
	m_feedbackVal = 0.0;
	m_position_1 = 0;

}

PidTunerWidget::~PidTunerWidget()
{
	ao();
#ifndef NOT_A_KOVAN
	publish();
#endif

}

double PidTunerWidget::getFeedbackValue()
{
#ifndef NOT_A_KOVAN
	publish();
#endif
	int position = get_motor_position_counter(ui->motor->currentIndex());

	double vel = (position - m_position_1) / MOTOR_SCALING;

	m_position_1 = position;

	return vel;
}

void PidTunerWidget::mouseEvent(double y)
{
	if (fabs(y - m_setpointVal) < 0.20) return;

	mav(ui->motor->currentIndex(), MOTOR_SCALING * y); // TODO: 1200?
#ifndef NOT_A_KOVAN
	publish();
#endif
	m_setpointVal = y;
}

void PidTunerWidget::update()
{

#ifndef NOT_A_KOVAN
	publish();
#endif

	ui->plot->push(m_feedback, getFeedbackValue());
	ui->plot->push(m_setpoint, m_setpointVal);
	ui->plot->inc();
}


void PidTunerWidget::coeffChanged()
{
	set_pid_gains(ui->motor->currentIndex(),
			ui->p->text().toDouble()*100,
			ui->i->text().toDouble()*100,
			ui->d->text().toDouble()*100,
			100,
			100,
			100);

#ifndef NOT_A_KOVAN
	publish();
#endif
}


void PidTunerWidget::motorChanged()
{
	// TODO clear prev position and stuff
}
