#include "Create3Widget.h"
#include "ui_Create3Widget.h"

#include <kipr/create3/client/client.h>

#include "NumpadDialog.h"
#include <QDebug>
#include <stdio.h>
#include <QRegularExpression>
#include <kipr/motor/motor.h>
#include <QProcess>
#include <cmath>
#include <QTimer>

#define MOTOR_SCALING (1500.0)
#define UPDATE_MS (25.0)
#define LPF_ALPHA (0.5)
int connected;

Create3Widget::Create3Widget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::Create3Widget)
{
    ui->setupUi(this);
    performStandardSetup(tr("Create 3"), false);

    // connect(ui->CreateConnectButton, SIGNAL(clicked()), SLOT(createConnect()));
    connect(ui->testProgramComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
    connect(ui->CreateConnectButton, SIGNAL(clicked()), SLOT(isConnected()));
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
        // QProcess process;
        // process.setWorkingDirectory("/home/erin/Desktop/qt6Upgrade/create3/build/client/examples");
        QString command = "./01_connect";
        QStringList arguments;
        arguments << "";

        process.start(command, arguments);

        if (process.waitForStarted())
        {
            // Process started successfully
            qDebug() << "Process started.";

            // Create a QString to store the standard output
            QString output;

            // Connect the readyReadStandardOutput signal to a slot or function
            QObject::connect(&process, &QProcess::readyReadStandardOutput, [&]()
                             {
            // Read and append the output to the QString
            output += process.readAllStandardOutput(); });

            // Wait for the process to finish
            process.waitForFinished();

            // Check the exit code/status
            int exitCode = process.exitCode();

            qDebug() << "Exit code: " << exitCode;
            QList<QString> inputList = output.split('\n');
            QRegularExpression regex("Output (\\d+)");

            for (const QString &input : inputList)
            {
                qDebug() << input;
                QRegularExpressionMatch match = regex.match(input);
                if (match.hasMatch())
                {
                    // Extract the number captured by the regular expression
                    QString numberStr = match.captured(1);
                    bool ok;
                    int number = numberStr.toInt(&ok);
                    if (ok)
                    {
                        // Process the extracted number
                        qDebug() << "Found number after 'Output': " << number;
                        break;
                    }
                }
            }
        }
        else
        {
            // Failed to start the process
            qDebug() << "Failed to start process.";
        }

        break;
    }

    case 1: // 02_Dock_Undock
    {

        QString command = "./02_dock_undock";
        QStringList arguments;
        arguments << "";

        process.start(command, arguments);

        if (process.waitForStarted())
        {
            // Process started successfully
            qDebug() << "Process started.";

            // Create a QString to store the standard output
            QString output;

            // Connect the readyReadStandardOutput signal to a slot or function
            QObject::connect(&process, &QProcess::readyReadStandardOutput, [&]()
                             {
            // Read and append the output to the QString
            output += process.readAllStandardOutput(); });

            // Wait for the process to finish
            process.waitForFinished();

            // Check the exit code/status
            int exitCode = process.exitCode();

            qDebug() << "Exit code: " << exitCode;
            QList<QString> inputList = output.split('\n');
        }
        else
        {
            // Failed to start the process
            qDebug() << "Failed to start process.";
        }
    }
    break;
    case 2: // 03_Move_Velocity
    {process.setWorkingDirectory("/home/erin/Desktop/qt6Upgrade/create3/build/client/examples");
        QString command = "./03_move_velocity";
        qDebug() << "command: " << command;
        QStringList arguments;
        arguments << "";

        process.start(command, arguments);

        if (process.waitForStarted())
        {
            // Process started successfully
            qDebug() << "Process started.";

            // Create a QString to store the standard output
            QString output;

            // Connect the readyReadStandardOutput signal to a slot or function
            QObject::connect(&process, &QProcess::readyReadStandardOutput, [&]()
                             {
            // Read and append the output to the QString
            output += process.readAllStandardOutput(); });

            // Wait for the process to finish
            process.waitForFinished();

            // Check the exit code/status
            int exitCode = process.exitCode();

            qDebug() << "Exit code: " << exitCode;
            QList<QString> inputList = output.split('\n');
        }
        else
        {
            // Failed to start the process
            qDebug() << "Failed to start process.";
        }
    }
    break;
    case 3: // 04_Rotate
    {process.setWorkingDirectory("/home/erin/Desktop/qt6Upgrade/create3/build/client/examples");
        QString command = "./04_rotate";
        qDebug() << "command: " << command;
        QStringList arguments;
        arguments << "";

        process.start(command, arguments);

        if (process.waitForStarted())
        {
            // Process started successfully
            qDebug() << "Process started.";

            // Create a QString to store the standard output
            QString output;

            // Connect the readyReadStandardOutput signal to a slot or function
            QObject::connect(&process, &QProcess::readyReadStandardOutput, [&]()
                             {
            // Read and append the output to the QString
            output += process.readAllStandardOutput(); });

            // Wait for the process to finish
            process.waitForFinished();

            // Check the exit code/status
            int exitCode = process.exitCode();

            qDebug() << "Exit code: " << exitCode;
            QList<QString> inputList = output.split('\n');
        }
        else
        {
            // Failed to start the process
            qDebug() << "Failed to start process.";
        }
    }
    break;
    case 4: // 05_Arc
    {process.setWorkingDirectory("/home/erin/Desktop/qt6Upgrade/create3/build/client/examples");
        QString command = "./05_arc";
        qDebug() << "command: " << command;
        QStringList arguments;
        arguments << "";

        process.start(command, arguments);

        if (process.waitForStarted())
        {
            // Process started successfully
            qDebug() << "Process started.";

            // Create a QString to store the standard output
            QString output;

            // Connect the readyReadStandardOutput signal to a slot or function
            QObject::connect(&process, &QProcess::readyReadStandardOutput, [&]()
                             {
            // Read and append the output to the QString
            output += process.readAllStandardOutput(); });

            // Wait for the process to finish
            process.waitForFinished();

            // Check the exit code/status
            int exitCode = process.exitCode();

            qDebug() << "Exit code: " << exitCode;
            QList<QString> inputList = output.split('\n');
        }
        else
        {
            // Failed to start the process
            qDebug() << "Failed to start process.";
        }
    }
    break;
    case 5: // 06_led_test
    {
         QProcess process;
        process.setWorkingDirectory("/home/erin/Desktop/qt6Upgrade/create3/build/client/examples");
        QString command = "./06_led_test";
    qDebug() << "command: " << command;
        QStringList arguments;
        arguments << "";

        process.start(command, arguments);

        if (process.waitForStarted())
        {
            // Process started successfully
            qDebug() << "Process started.";

            // Create a QString to store the standard output
            QString output;

            // Connect the readyReadStandardOutput signal to a slot or function
            QObject::connect(&process, &QProcess::readyReadStandardOutput, [&]()
                             {
            // Read and append the output to the QString
            output += process.readAllStandardOutput(); });

            // Wait for the process to finish
            process.waitForFinished();

            // Check the exit code/status
            int exitCode = process.exitCode();

            qDebug() << "Exit code: " << exitCode;
            QList<QString> inputList = output.split('\n');
            QRegularExpression regex("Output (\\d+)");
            qDebug() << "Output == " << inputList;
            for (const QString &input : inputList)
            {
                qDebug() << input;
                QRegularExpressionMatch match = regex.match(input);
                if (match.hasMatch())
                {
                    // Extract the number captured by the regular expression
                    QString numberStr = match.captured(1);
                    bool ok;
                    int number = numberStr.toInt(&ok);
                    if (ok)
                    {
                        // Process the extracted number
                        qDebug() << "Found number after 'Output': " << number;
                        break;
                    }
                }
            }
        }
        else
        {
            // Failed to start the process
            qDebug() << "Failed to start process.";
        }

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
