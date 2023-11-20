#include "Create3Widget.h"
#include "ui_Create3Widget.h"
#include "Create3SensorListWidget.h"
#include "Create3ExampleWidget.h"
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
    connect(ui->Create3ExampleProgramButton, SIGNAL(clicked()), SLOT(exampleList()));
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

void Create3Widget::exampleList()
{
    RootController::ref().presentWidget(new Create3ExampleWidget(device()));
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



