#include "Create3Widget.h"
#include "ui_Create3Widget.h"
#include "Create3SensorListWidget.h"
#include "SensorModel.h"
#include <kipr/create3/client/client.h>
#include <unistd.h>
#include "NumpadDialog.h"
#include <QDebug>
#include <stdio.h>
#include <QRegularExpression>
#include <kipr/motor/motor.h>
#include <QProcess>
#include <QHeaderView>
#include <cmath>
#include <QTimer>
#include <QPainter>
#include <QItemDelegate>
#include "RootController.h"

#define MOTOR_SCALING (1500.0)
#define UPDATE_MS (25.0)
#define LPF_ALPHA (0.5)
int connected;

class SensorItemDelegate : public QItemDelegate
{
public:
    SensorItemDelegate(SensorModel *const model, QObject *const parent = 0);
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    const SensorModel *const _model;
    const QPixmap _up;
    const QPixmap _down;
};

Create3Widget::Create3Widget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::Create3Widget), _model(new SensorModel(this))
{
    ui->setupUi(this);
    performStandardSetup(tr("Create 3"), false);
    setStyleSheet("QScrollBar:vertical {border: 2px solid grey;background:grey ;width: 100px; margin: 22px 0 22px 0; } QScrollBar::handle:vertical { border: 2px solid grey;background: white; min-height: 20px; } QScrollBar::add-line:vertical { border: 2px solid grey;background: #32CC99; height: 20px; subcontrol-position: bottom; subcontrol-origin: margin; } QScrollBar::sub-line:vertical {border: 2px solid grey; background: #32CC99; height: 20px; subcontrol-position: top; subcontrol-origin: margin; } QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { border: 2px solid grey;width: 3px; height: 3px; background: white; }QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {border: 2px solid grey; background: none;}");
    connect(ui->CreateConnectButton, SIGNAL(clicked()), SLOT(isConnected()));

    // ui->sensors->setModel(_model);
    // ui->sensors->setItemDelegate(new SensorItemDelegate(_model, this));
    // connect(ui->testProgramComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
    connect(ui->Create3SensorListButton, SIGNAL(clicked()), SLOT(sensorList()));
}

Create3Widget::~Create3Widget()
{
    delete ui;
}

int Create3Widget::isConnected()
{
    connected = create3_is_connected();
    if (connected == 1)
    {
        qDebug() << "Create connected";
        ui->createConnectState->setText("Connected");
    }
    else
    {
        qDebug() << "Create not connected";
        ui->createConnectState->setText("Not Connected");
    }
    return connected;
}

void Create3Widget::sensorList()
{
    RootController::ref().presentWidget(new Create3SensorListWidget(device()));
}

int Create3Widget::createConnect()
{
    // create3_connect();
    int connected = create3_connect_manual("192.168.125.1", 50051);
    qDebug() << "Create connected? " << connected;
    if (connected == 1)
    {

        return true;
    }
    else
    {
        return false;
    }
}

void Create3Widget::indexChanged(int index)
{
    QProcess process;
    process.setWorkingDirectory("/home/erin/Desktop/qt6Upgrade/create3/build/client/examples");
    switch (index)
    {

    case 0:
    { // 01_Connect
        printf("Connecting to Create3 robot...\n");
        create3_connect_manual("192.168.125.1", 50051);
        printf("Connected!\n");
        break;
    }

    case 1: // 02_Dock_Undock
    {

        printf("Connecting to Create3 robot...\n");

        create3_connect_manual("192.168.125.1", 50051);

        printf("Connected!\n");

        printf("Undocking...\n");
        // Move forward at 0.5 m/s for 1 second
        create3_undock();

        create3_wait();

        printf("Docking...\n");

        create3_dock();

        create3_wait();

        printf("Done!\n");

        break;
    }

    case 2: // 03_Move_Velocity
    {
        printf("Connecting to Create3 robot...\n");

        create3_connect_manual("192.168.125.1", 50051);

        printf("Connected!\n");

        printf("Moving forward at 0.5 m/s for 1 second...\n");

        // Move forward at 0.5 m/s for 1 second
        create3_velocity_set_components(0.5, 0);

        // Wait for 1 second
        usleep(1000000);

        // Stop
        create3_velocity_set_components(0.0, 0.0);

        printf("Done!\n");
        break;
    }

    case 3: // 04_Rotate
    {
        printf("Connecting to Create3 robot...\n");

        create3_connect_manual("192.168.125.1", 50051);

        printf("Connected!\n");

        printf("Rotate at 0.5 m/s for 1 second...\n");

        // Rotate at 90 deg/s (PI/2 rad/s) to 90 degrees (PI/2 radians)
        create3_rotate_degrees(90, 90);
        create3_rotate_radians(1.57, 1.57);

        // Wait for 1 second
        create3_wait();

        printf("Done!\n");

        break;
    }

    case 4: // 05_Arc
    {
        printf("Connecting to Create3 robot...\n");

        create3_connect_manual("192.168.125.1", 50051);

        printf("Connected!\n");

        printf("Rotate at 0.5 m/s for 1 second...\n");

        // Arc of 90 deg (PI/2 rad) with radius of 0.5 m
        create3_drive_arc_degrees(0.5, 90, 0.306);
        create3_drive_arc_radians(0.5, -1.57, 0.460);

        // Wait for 1 second
        create3_wait();

        printf("Done!\n");

        break;
    }

    case 5: // 06_led_test
    {
        printf("Connecting to Create3 robot...\n");

        create3_connect_manual("192.168.125.1", 50051);

        printf("Connected!\n");

        printf("Blinking lights...\n");
        // Blink the lights green for 5 seconds.

        Create3LedColor green;
        green.r = 0;
        green.g = 255;
        green.b = 0;

        Create3Lightring lightring;
        lightring.led0 = green;
        lightring.led1 = green;
        lightring.led2 = green;
        lightring.led3 = green;
        lightring.led4 = green;
        lightring.led5 = green;

        create3_led_animation(Create3BlinkLights, lightring, 5.0); // Blink the lights green for 5 seconds.

        // Wait for 1 second
        create3_wait();

        printf("Done!\n");

        break;
    }

    default:
        break;
    }
}

void Create3Widget::update()
{
}

void Create3Widget::coeffChanged()
{
}

void Create3Widget::motorChanged()
{
}

void Create3Widget::updatePids()
{
}
