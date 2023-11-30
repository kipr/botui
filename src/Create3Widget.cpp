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
    connect(ui->Create3SensorListButton, SIGNAL(clicked()), SLOT(sensorList()));
    connect(ui->Create3ExampleProgramButton, SIGNAL(clicked()), SLOT(exampleList()));

    // startCreate3Service = new QProcess(this);
    stopCreate3Service = new QProcess(this);
   
    ui->create3IP->setText(getIP());
}

Create3Widget::~Create3Widget()
{
    delete ui;
}

QString Create3Widget::getIP()
{

     // The command to execute
    const char *command = "arp -a | grep 'iRobot' | awk -F '[()]' '{print $2}'";

    // Open a pipe to the command
    FILE *pipe = popen(command, "r");
    if (!pipe)
    {
        std::cerr << "Error opening pipe." << std::endl;
    }

    // Read the output from the command
    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }

    // Close the pipe
    pclose(pipe);

    // Trim the newline character from the end of the result
    result.erase(result.find_last_not_of("\n") + 1);

    // Print or use the result as needed
    std::cout << "Output:\n"
              << result << std::endl;


    QString output = QString::fromStdString(result);

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
    QString stopCommand = "sudo";
    QStringList stopArgs = {
        "systemctl",
        "stop",
        "create3_server.service"
    };

    connect(stopCreate3Service, &QProcess::readyReadStandardError, [=]() {
        QByteArray data = stopCreate3Service->readAllStandardError();
        qDebug() << "Error:" << data;
    });

    // Start process to stop server
    stopCreate3Service->start(stopCommand, stopArgs);

    if (stopCreate3Service->waitForFinished())
    {
        qDebug() << "Create3 Server successfully stopped";
    }
    else
    {
        qDebug() << "Create3 Server failed to stop or crashed.";
    }

    // QString startCommand = "sudo";
    // QStringList startArgs = {
    //     "systemctl",
    //     "start",
    //     "create3_server.service"
    // };

    // connect(startCreate3Service, &QProcess::readyReadStandardError, [=]() {
    //     QByteArray data = startCreate3Service->readAllStandardError();
    //     qDebug() << "Error:" << data;
    // });

    // // Start process to start server
    // if (startCreate3Service->startDetached(startCommand, startArgs))
    // {
    //     qDebug() << "Create3 Server successfully started";
    // }
    // else
    // {
    //     qDebug() << "Create3 Server failed to start or crashed.";
    // }
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
        connected = create3_connect();
        qDebug() << "Create connected? " << connected;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return connected;
}
