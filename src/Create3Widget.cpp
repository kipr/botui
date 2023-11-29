#include "Create3Widget.h"
#include "ui_Create3Widget.h"
#include "Create3SensorListWidget.h"
#include "Create3ExampleWidget.h"
#include "Create3SensorModel.h"
#include <kipr/create3/client/client.h>
#include <unistd.h>
#include "NumpadDialog.h"
#include <QDebug>
#include <stdio.h>
#include <iostream>
#include <QRegularExpression>
#include <QProcess>
#include <QHeaderView>
#include <cmath>
#include <QTimer>
#include <QPainter>
#include <QItemDelegate>
#include "RootController.h"

class Create3SensorItemDelegate : public QItemDelegate
{
public:
    Create3SensorItemDelegate(Create3SensorModel *const model, QObject *const parent = 0);
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    const Create3SensorModel *const _model;
    const QPixmap _up;
    const QPixmap _down;
};

Create3Widget::Create3Widget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::Create3Widget), _model(new Create3SensorModel(this))
{
    ui->setupUi(this);
    performStandardSetup(tr("Create 3"), false);
    setStyleSheet("QScrollBar:vertical {border: 2px solid grey;background:grey ;width: 100px; margin: 22px 0 22px 0; } QScrollBar::handle:vertical { border: 2px solid grey;background: white; min-height: 20px; } QScrollBar::add-line:vertical { border: 2px solid grey;background: #32CC99; height: 20px; subcontrol-position: bottom; subcontrol-origin: margin; } QScrollBar::sub-line:vertical {border: 2px solid grey; background: #32CC99; height: 20px; subcontrol-position: top; subcontrol-origin: margin; } QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { border: 2px solid grey;width: 3px; height: 3px; background: white; }QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {border: 2px solid grey; background: none;}");
    connect(ui->CreateConnectButton, SIGNAL(clicked()), SLOT(isConnected()));
    connect(ui->ResetServerButton, SIGNAL(clicked()), SLOT(resetServer()));
    // ui->sensors->setModel(_model);
    // ui->sensors->setItemDelegate(new Create3SensorItemDelegate(_model, this));
    // connect(ui->testProgramComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
    connect(ui->Create3SensorListButton, SIGNAL(clicked()), SLOT(sensorList()));
    connect(ui->Create3ExampleProgramButton, SIGNAL(clicked()), SLOT(exampleList()));

    podmanStop = new QProcess(this);
    podmanStart = new QProcess(this);

    QStringList arguments;
    arguments << "-a"
              << "| grep 'iRobot' | awk -F '[()]' '{print $2}'";

    QProcess *myProcess = new QProcess(parent);
    myProcess->start("arp", arguments);
    myProcess->waitForFinished();
    QByteArray output = myProcess->readAllStandardOutput();
    ui->create3IP->setText(output);
}

Create3Widget::~Create3Widget()
{
    delete ui;
}

QString Create3Widget::getIP()
{
    QProcess *myProc = new QProcess();
    QStringList args;
    myProc->start("arp -a | grep 'iRobot' | awk -F '[()]' '{print $2}'");
    myProc->waitForFinished();
    QByteArray output = myProc->readAllStandardOutput();
    qDebug() << "Create3 IP: " << output;

    return output;
}

int Create3Widget::isConnected()
{
    int connected;
    try
    {
        connected = create3_is_connected();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
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

void Create3Widget::resetServer()
{
    QString podmanStopCommand = "sudo";
    QStringList podmanStopArgs = {
        "podman",
        "stop",
        "-a"
    };

    connect(podmanStop, &QProcess::readyReadStandardOutput, [=]() {
        QByteArray data = podmanStop->readAllStandardOutput();
        qDebug() << "Output:" << data;
    });

    connect(podmanStop, &QProcess::readyReadStandardError, [=]() {
        QByteArray data = podmanStop->readAllStandardError();
        qDebug() << "Error:" << data;
    });

    podmanStop->startDetached(podmanStopCommand, podmanStopArgs);

    if (podmanStop->waitForFinished())
    {
        qDebug() << "Podman container successfully stopped with exit code:" << podmanStop->exitCode();
    }
    else
    {
        qDebug() << "Podman stop failed to start or crashed.";
    }

    QString podmanStartCommand = "sudo";
    QStringList podmanStartArgs = {
        "podman",
        "run",
        "-it",
        "--rm",
        "--net=host",
        "--env",
        "IP=192.168.125.1",
        "docker.io/kipradmin/create3_docker"
    };

    connect(podmanStart, &QProcess::readyReadStandardOutput, [=]() {
        QByteArray data = podmanStart->readAllStandardOutput();
        qDebug() << "Output:" << data; 
    });

    connect(podmanStart, &QProcess::readyReadStandardError, [=]() {
        QByteArray data = podmanStart->readAllStandardError();
        qDebug() << "Error:" << data;
    });

    podmanStart->startDetached(podmanStartCommand, podmanStartArgs);

    if (podmanStart->waitForFinished())
    {
        qDebug() << "Podman container successfully started with exit code:" << podmanStart->exitCode();
    }
    else
    {
        qDebug() << "Podman container failed to start or crashed.";
    }
}

void Create3Widget::sensorList()
{
    RootController::ref().presentWidget(new Create3SensorListWidget(device()));
}

void Create3Widget::exampleList()
{
    RootController::ref().presentWidget(new Create3ExampleWidget(device()));
}

int Create3Widget::create3Connect()
{
    int connected = 0;
    try
    {
        connected = create3_connect_manual("192.168.125.1", 50051);
        qDebug() << "Create connected? " << connected;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return connected;
}
