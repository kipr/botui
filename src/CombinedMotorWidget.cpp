#include "CombinedMotorWidget.h"
#include "ui_CombinedMotorWidget.h"

#include <QDebug>
#include <QTimer>

#include <math.h>

#include <kipr/kipr.h>

#include "MenuBar.h"
#include "NumpadDialog.h"

#define A_KOVAN

CombinedMotorWidget::CombinedMotorWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent), ui(new Ui::CombinedMotorWidget) {
    ui->setupUi(this);

    performStandardSetup(tr("Motors"), false);

    QAction *clearPosition = menuBar()->addAction(tr("Clear Position"));
    connect(clearPosition, SIGNAL(triggered()), SLOT(clearPosition()));

    ui->pwm->setMinimumValue(-100.0);
    ui->pwm->setMaximumValue(100.0);
    ui->pwm->setValue(0.0);

    ui->velocity->setMinimumValue(-1500.0);
    ui->velocity->setMaximumValue(1500.0);
    ui->velocity->setValue(0.0);

    connect(ui->pwm, SIGNAL(valueChanged(double)), SLOT(pwmChanged(double)));
    connect(
        ui->velocity, SIGNAL(valueChanged(double)), SLOT(velChanged(double)));

    connect(ui->pwmStop, SIGNAL(clicked()), SLOT(stop()));
    connect(ui->velocityStop, SIGNAL(clicked()), SLOT(stop()));
    connect(ui->positionStop, SIGNAL(clicked()), SLOT(stop()));

    connect(ui->fd, SIGNAL(clicked()), SLOT(forward()));
    connect(ui->bk, SIGNAL(clicked()), SLOT(backward()));

    connect(ui->tabs, SIGNAL(currentChanged(int)), SLOT(stop()));

    // TODO: remove this once position mode works
    ui->tabs->removeTab(2);

    connect(ui->go, SIGNAL(clicked()), SLOT(go()));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(update()));
    timer->start(100);

    ui->goalPos->setInputProvider(
        new NumpadDialog(tr("Goal Position"),
                         NumpadDialog::Integer,
                         std::numeric_limits<double>::min(),
                         std::numeric_limits<double>::max(),
                         this));
    ui->speed->setInputProvider(new NumpadDialog(
        tr("Speed"), NumpadDialog::Integer, -1500, 1500, this));

    ao();
}

CombinedMotorWidget::~CombinedMotorWidget() {
    ao();

    delete ui;
}

void CombinedMotorWidget::pwmChanged(double pwm) {
    ui->fd->setEnabled(pwm < 99.5);
    ui->bk->setEnabled(pwm > -99.5);
    ui->pwmStop->setEnabled(qAbs(pwm) > 0.5);
    motor_power(ui->motors->currentIndex(), pwm);
}

void CombinedMotorWidget::velChanged(double vel) {
    ui->velocityStop->setEnabled(qAbs(vel) > 0.5);
    move_at_velocity(ui->motors->currentIndex(), vel);
}

void CombinedMotorWidget::tabChanged(int i) { stop(); }

void CombinedMotorWidget::go() {
    move_to_position(ui->motors->currentIndex(),
                     ui->speed->text().toInt(),
                     ui->goalPos->text().toInt());
}

void CombinedMotorWidget::forward() { ui->pwm->setValue(100.0); }

void CombinedMotorWidget::backward() { ui->pwm->setValue(-100.0); }

void CombinedMotorWidget::update() {

    int port = ui->motors->currentIndex();
    ui->position->setText(QString::number(get_motor_position_counter(port)));
    ui->positionStop->setEnabled(get_motor_done(port) ? false : true);
    ui->go->setEnabled(get_motor_done(port) ? true : false);
}

void CombinedMotorWidget::clearPosition() {
    clear_motor_position_counter(ui->motors->currentIndex());
}

void CombinedMotorWidget::stop() {
    off(ui->motors->currentIndex());
    ui->velocity->setValue(0);
    ui->pwm->setValue(0);
}
