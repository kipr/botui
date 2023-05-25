#include "PidTunerWidget.h"
#include "ui_PidTunerWidget.h"

#include "NumpadDialog.h"
#include <QDebug>

#include <kipr/motor/motor.h>

#include <cmath>

#define MOTOR_SCALING (1500.0)
#define UPDATE_MS (25.0)
#define LPF_ALPHA (0.5)

PidTunerWidget::PidTunerWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent), ui(new Ui::PidTunerWidget) {
    ui->setupUi(this);
    performStandardSetup(tr("PID Tuner"), false);
    connect(ui->p, SIGNAL(textChanged(QString)), SLOT(coeffChanged()));
    connect(ui->i, SIGNAL(textChanged(QString)), SLOT(coeffChanged()));
    connect(ui->d, SIGNAL(textChanged(QString)), SLOT(coeffChanged()));
    connect(ui->motor, SIGNAL(currentIndexChanged(int)), SLOT(motorChanged()));
    // void get_pid_gains(int motor, short *p, short *i, short *d, short *pd,
    // short *id, short *dd)

    NumpadDialog *pProvider = new NumpadDialog(
        tr("P Value"), NumpadDialog::Decimal, -99.0, 99.0, this);
    NumpadDialog *iProvider = new NumpadDialog(
        tr("I Value"), NumpadDialog::Decimal, -99.0, 99.0, this);
    NumpadDialog *dProvider = new NumpadDialog(
        tr("D Value"), NumpadDialog::Decimal, -99.0, 99.0, this);

    ui->p->setInputProvider(pProvider);
    ui->i->setInputProvider(iProvider);
    ui->d->setInputProvider(dProvider);

    m_feedback = ui->plot->addPlot(QColor(0, 0, 200));
    m_setpoint = ui->plot->addPlot(QColor(0, 0, 0));

    updatePids();

    QTimer *updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), SLOT(update()));
    updateTimer->start(UPDATE_MS);

    connect(ui->plot, SIGNAL(mouseEvent(double)), SLOT(mouseEvent(double)));

    m_setpointVal = 0.0;
    m_feedbackVal = 0.0;
    m_position_1 = 0;
    m_vel_1 = 0;
}

PidTunerWidget::~PidTunerWidget() { ao(); }

double PidTunerWidget::getFeedbackValue() {
    int position = get_motor_position_counter(ui->motor->currentIndex());

    double vel = (1.0 - LPF_ALPHA) * m_vel_1 + LPF_ALPHA * (1000 / UPDATE_MS) *
                                                   (position - m_position_1) /
                                                   MOTOR_SCALING;

    m_position_1 = position;
    m_vel_1 = vel;

    return vel;
}

void PidTunerWidget::mouseEvent(double y) {
    if (fabs(y - m_setpointVal) < 0.20) return;

    mav(ui->motor->currentIndex(), MOTOR_SCALING * y); // TODO: 1200?
    m_setpointVal = y;
}

void PidTunerWidget::update() {
    ui->plot->push(m_feedback, getFeedbackValue());
    ui->plot->push(m_setpoint, m_setpointVal);
    ui->plot->inc();
}

void PidTunerWidget::coeffChanged() {
    set_pid_gains(ui->motor->currentIndex(),
                  ui->p->text().toDouble() * 100,
                  ui->i->text().toDouble() * 100,
                  ui->d->text().toDouble() * 100,
                  100,
                  100,
                  100);
}

void PidTunerWidget::motorChanged() { updatePids(); }

void PidTunerWidget::updatePids() {
    short p = 0;
    short i = 0;
    short d = 0;
    short pd = 0;
    short id = 0;
    short dd = 0;

    get_pid_gains(ui->motor->currentIndex(), &p, &i, &d, &pd, &id, &dd);

    char buff[80];

    sprintf(buff, "%f\n", ((double)p / (double)pd));
    ui->p->setText(buff);

    sprintf(buff, "%f\n", ((double)i / (double)id));
    ui->i->setText(buff);

    sprintf(buff, "%f\n", ((double)d / (double)dd));
    ui->d->setText(buff);
}
