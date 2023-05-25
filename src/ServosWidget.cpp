#include "ServosWidget.h"
#include "Device.h"
#include "MenuBar.h"
#include "NumpadDialog.h"
#include "RootController.h"
#include "StatusBar.h"
#include "ui_ServosWidget.h"

#include <kipr/servo/servo.h>

#include <QDebug>
#include <math.h>

ServosWidget::ServosWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::ServosWidget),
      m_provider(new NumpadDialog(QString())) {
    ui->setupUi(this);
    performStandardSetup(tr("Servos"));

    ui->number->setInputProvider(m_provider);

    connect(ui->slider, SIGNAL(valueChanged(int)), SLOT(positionChanged(int)));
    connect(
        ui->number, SIGNAL(textEdited(QString)), SLOT(manualEntry(QString)));
    connect(ui->port_0, SIGNAL(clicked()), SLOT(activeChanged()));
    connect(ui->port_1, SIGNAL(clicked()), SLOT(activeChanged()));
    connect(ui->port_2, SIGNAL(clicked()), SLOT(activeChanged()));
    connect(ui->port_3, SIGNAL(clicked()), SLOT(activeChanged()));
    connect(ui->enable, SIGNAL(clicked()), SLOT(enable()));
    connect(ui->disable, SIGNAL(clicked()), SLOT(disable()));

    // Setup ui for port 0 (the default servo)
    ui->slider->setMinimum(0);
    ui->slider->setMaximum(2047);
    ui->slider->setValue(get_servo_position(0));
    ui->slider->setLabel(0);
    ui->port_0->setEnabled(false);

    const bool servoEnabled = get_servo_enabled(0);
    ui->enable->setVisible(!servoEnabled);
    ui->disable->setVisible(servoEnabled);
}

ServosWidget::~ServosWidget() {
    delete ui;
    delete m_provider;
}

void ServosWidget::positionChanged(const int &value) {
    // Keep position label in sync with dial
    ui->number->setText(QString::number(value));

    set_servo_position(ui->slider->label(), value);
}

void ServosWidget::activeChanged() {
    QObject *from = sender();

    // Set dial label for new servo
    int newPort = -1;
    if (from == ui->port_0)
        newPort = 0;
    else if (from == ui->port_1)
        newPort = 1;
    else if (from == ui->port_2)
        newPort = 2;
    else if (from == ui->port_3)
        newPort = 3;
    else
        return;
    ui->slider->setLabel(newPort);

    // Set dial value for new servo
    ui->slider->setValue(get_servo_position(newPort));

    // Enable/disable appropriate port buttons
    ui->port_0->setEnabled(from != ui->port_0);
    ui->port_1->setEnabled(from != ui->port_1);
    ui->port_2->setEnabled(from != ui->port_2);
    ui->port_3->setEnabled(from != ui->port_3);

    // Show/hide enable and disable buttons
    const bool servoEnabled = get_servo_enabled(newPort);
    ui->enable->setVisible(!servoEnabled);
    ui->disable->setVisible(servoEnabled);
}

void ServosWidget::enable() {
    // Enable current servo
    const int port = ui->slider->label();
    enable_servo(port);

    // Update enable/disable button visibility
    ui->enable->hide();
    ui->disable->show();
}

void ServosWidget::disable() {
    // Disable current servo
    const int port = ui->slider->label();
    disable_servo(port);

    // Update enable/disable button visibility
    ui->enable->show();
    ui->disable->hide();
}

void ServosWidget::manualEntry(const QString &text) {
    // Keep dial in sync with position label
    ui->slider->setValue(text.toInt());
}
