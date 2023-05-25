#include "SensorsWidget.h"
#include "Device.h"
#include "MenuBar.h"
#include "NetworkItemModel.h"
#include "NetworkManager.h"
#include "OtherNetworkWidget.h"
#include "RootController.h"
#include "StatusBar.h"
#include "ui_SensorsWidget.h"
#include <QTimer>

#include <kipr/kipr.h>

#include <QDebug>

#include <math.h> // Temporary

SensorsWidget::SensorsWidget(Device *device, QWidget *parent)
    : QWidget(parent), m_menuBar(new MenuBar(this)), ui(new Ui::SensorsWidget) {
    ui->setupUi(this);
    m_menuBar->addHomeAndBackButtons();
    m_menuBar->setTitle(tr("Sensors"));
    layout()->setMenuBar(m_menuBar);

    m_plots[0] = ui->plot->addPlot(QColor(200, 0, 0));
    m_plots[1] = ui->plot->addPlot(QColor(0, 0, 200));

    QTimer *updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), SLOT(update()));
    updateTimer->start(10);
}

SensorsWidget::~SensorsWidget() { delete ui; }

void SensorsWidget::update() {
    ui->val1->setText(QString::number(rawValue(ui->plot1->currentIndex())));
    ui->val2->setText(QString::number(rawValue(ui->plot2->currentIndex())));

    ui->plot->push(m_plots[0], value(ui->plot1->currentIndex()));
    ui->plot->push(m_plots[1], value(ui->plot2->currentIndex()));
    ui->plot->inc();
}

double SensorsWidget::rawValue(const int &i) const {
    double val = 0;
#ifdef WOMBAT
    if (i < 6)
        val = analog(i);
    else if (i < 10)
        val = get_motor_position_counter(i - 6);
    else if (i == 10)
        val = accel_x();
    else if (i == 11)
        val = accel_y();
    else if (i == 12)
        val = accel_z();
    else if (i == 13)
        val = gyro_x();
    else if (i == 14)
        val = gyro_y();
    else if (i == 15)
        val = gyro_z();
    else if (i == 16)
        val = magneto_x();
    else if (i == 17)
        val = magneto_y();
    else if (i == 18)
        val = magneto_z();
    else if (i == 19)
        val = left_button();
    else if (i == 20) {
        if (right_button() == true)
            val = false;
        else if (right_button() == false)
            val = true;
    }
#else
    if (i < 8)
        val = analog(i);
    else if (i < 12)
        val = get_motor_position_counter(i - 8);
    else if (i == 12)
        val = accel_x();
    else if (i == 13)
        val = accel_y();
    else if (i == 14)
        val = accel_z();
#endif
    return val;
}

double SensorsWidget::value(const int &i) const {
    double val = rawValue(i);

#ifdef WOMBAT
    if (i < 6) // analog readings
        val = val / 2048.0 - 1.0;
    else if (i < 10) // gmpc readings
        val = val / 32768.0;
    else if (i < 19) // IMU readings
        val = val / 32768.0;
    else
        val = 0.0;
#else
    if (i < 8)
        val = val / 512.0 - 1.0;
    else if (i < 12)
        val = val / 32768.0;
    else if (i < 15)
        val = val / 512.0;
    else
        val = 0.0;
#endif

    return val;
}
