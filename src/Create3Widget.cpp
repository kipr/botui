#include "Create3Widget.h"
#include "ui_Create3Widget.h"
#include "Create3SensorListWidget.h"
#include "kj/exception.h"
#include "Create3SensorModel.h"
#include <kipr/create3/client/client.h>
#include <unistd.h>
#include "NumpadDialog.h"
#include <QDebug>
#include <stdio.h>
#include <iostream>
#include <QRegularExpression>
#include <QHeaderView>
#include <cmath>
#include <QTimer>
#include <QPainter>
#include <QItemDelegate>
#include "RootController.h"
#include "kipr/create3/create3.capnp.h"
#include <QMessageBox>
#include <QLabel>
#include <QMovie>
#include <QSize>
#include <QProcess>
#include <QVBoxLayout>
#include <QGridLayout>
int exampleIndex;
QStringList programList;

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

QByteArray output;

Create3Widget::Create3Widget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::Create3Widget), _model(new Create3SensorModel(this))
{
    ui->setupUi(this);
    performStandardSetup(tr("Create 3"), false);
    setStyleSheet("QScrollBar:vertical {border: 2px solid grey;background:grey ;width: 80px; margin: 22px 0 22px 0; } QScrollBar::handle:vertical { border: 2px solid grey;background: white; min-height: 20px; } QScrollBar::add-line:vertical { border: 2px solid grey;background: #32CC99; height: 20px; subcontrol-position: bottom; subcontrol-origin: margin; } QScrollBar::sub-line:vertical {border: 2px solid grey; background: #32CC99; height: 20px; subcontrol-position: top; subcontrol-origin: margin; } QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { border: 2px solid grey;width: 3px; height: 3px; background: white; }QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {border: 2px solid grey; background: none;}");

    ui->create3IP->setText(getIP());

    QStringList arguments;
    arguments << "/home/kipr/wombat-os/configFiles/create3_server_ip.txt";

    QProcess *myProcess = new QProcess(parent);
    myProcess->start("cat", arguments);
    myProcess->waitForFinished();
    output = myProcess->readAllStandardOutput();

    qDebug() << output;

    QString ipOutput = QString(output);

    if (ipOutput == "192.168.125.1")
    {
        ui->toggleSwitch->setChecked(false); // Wifi toggle side
    }
    else if (ipOutput == "192.168.186.3")
    {
        ui->toggleSwitch->setChecked(true); // Ethernet toggle side
    }

    connect(ui->toggleSwitch, SIGNAL(stateChanged(int)), this, SLOT(toggleChanged()));
    connect(ui->exampleComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));

    connect(ui->exampleRunButton, SIGNAL(clicked()), SLOT(run()));

    // Create model
    model = new QStringListModel(this);
    ui->listWidget->clear();
    programList.clear();
    programList << "printf('Connecting to Create3 robot...');"
                << "create3_connect_manual('192.168.125.1', 50051);"
                << "printf('Connected!');";

    ui->listWidget->addItems(programList);
    model->setStringList(programList);
    qDebug() << "Program List: " << programList;
    ui->listWidget->setSpacing(6);
}

Create3Widget::~Create3Widget()
{
    delete ui;
}

void Create3Widget::toggleChanged()
{

    StandardWidget::disableMenuBar();

    ui->toggleSwitch->setEnabled(false);
    ui->exampleComboBox->setEnabled(false);
    ui->exampleRunButton->setEnabled(false);

    QProcess checkCreate3IPState;
    QString startCommand = "cat";
    QStringList startArgs = {"/home/kipr/wombat-os/configFiles/create3_server_ip.txt"};

    checkCreate3IPState.start(startCommand, startArgs);
    checkCreate3IPState.waitForFinished();
    QByteArray output = checkCreate3IPState.readAllStandardOutput();

    QString ipOutput = QString(output);

    qDebug() << "IP OUTPUT: " << ipOutput; // Get current IP output
    

    if (ipOutput.contains("192.168.125.1"))
    {

        if (QMessageBox::question(this, "Change Interface?",
                                  QString("You are about to change your Create 3 connection from Wifi to Ethernet. \nThe Wombat will reboot once you make this change. \n Do you want to continue? \n (Be sure to change the Fast DDS discovery server IP address to 192.168.186.3)"),
                                  QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        {

            StandardWidget::enableMenuBar();
            ui->toggleSwitch->setChecked(false); // Wifi toggle side
            ui->toggleSwitch->setEnabled(true);
            ui->exampleComboBox->setEnabled(true);
            ui->exampleRunButton->setEnabled(true);

            return;
        }
        else
        {
            rebootBox();
            QProcess process;
            process.startDetached("/bin/sh", QStringList() << "/home/kipr/wombat-os/configFiles/create3_interface_swap.sh"
                                                           << "eth");
        }
    }
    else if (ipOutput.contains("192.168.186.3"))
    {
        if (QMessageBox::question(this, "Change Interface?",
                                  QString("You are about to change your Create 3 connection from Ethernet to Wifi. \n The Wombat will reboot once you make this change. \nDo you want to continue? \n (Be sure to change the Fast DDS discovery server IP address to 192.168.125.1)"),
                                  QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        {

            StandardWidget::enableMenuBar();
            ui->toggleSwitch->setChecked(true); // Ethernet toggle side
            ui->toggleSwitch->setEnabled(true);
            ui->exampleComboBox->setEnabled(true);
            ui->exampleRunButton->setEnabled(true);

            return;
        }
        else
        {
            rebootBox();
            QProcess process;
            process.startDetached("/bin/sh", QStringList() << "/home/kipr/wombat-os/configFiles/create3_interface_swap.sh"
                                                           << "wifi");
        }
    }
}

void Create3Widget::rebootBox()
{

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Reboot");
    msgBox.setText("Rebooting now...");

    // msgBox.setText("Rebooting...");
    msgBox.setMaximumSize(500, 480);
    // msgBox.setStyleSheet("QLabel{min-width: 450px; min-height: 280px;}");
    msgBox.setStandardButtons(QMessageBox::NoButton);

    QLabel *gifLabel = new QLabel();
    QLabel *messageLabel = new QLabel(msgBox.text());

    QGridLayout *msgBoxLayout = qobject_cast<QGridLayout *>(msgBox.layout());

    msgBoxLayout->setVerticalSpacing(0);

    QWidget *container = new QWidget();
    QVBoxLayout *vLayout = new QVBoxLayout(container);

    vLayout->addWidget(gifLabel);

    vLayout->addWidget(messageLabel);
    vLayout->setAlignment(Qt::AlignCenter);
    gifLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setAlignment(Qt::AlignCenter);

    msgBoxLayout->setSpacing(0);
    vLayout->setSpacing(10);

    container->setLayout(vLayout);

    if (msgBoxLayout)
    {
        msgBoxLayout->addWidget(container, 0, 0, 1, msgBoxLayout->columnCount());
    }

    gifLabel->move(200, -50);
    gifLabel->resize(400, 1100);

    QMovie *movie = new QMovie("://qml/botguy_noMargin.gif");
    movie->setScaledSize(QSize(200, 240));
    gifLabel->setMovie(movie);
    movie->start();
    gifLabel->show();

    msgBox.setText("");
    msgBox.exec();
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

// void Create3Widget::sensorList()
// {
//     RootController::ref().presentWidget(new Create3SensorListWidget(device()));
// }

// void Create3Widget::exampleList()
// {
//     RootController::ref().presentWidget(new Create3ExampleWidget(device()));
// }

void Create3Widget::indexChanged(int index)
{

    programList.clear();
    ui->listWidget->clear();
    ui->listWidget->addItems(programList);
    model->setStringList(programList);
    switch (index)
    {
    case 0: // 01_Connect
    {

        programList << "printf('Connecting to Create3 robot...');"
                    << "create3_connect_manual('192.168.125.1', 50051);"
                    << "printf('Connected!');";
        qDebug() << "Program List: " << programList;
        ui->listWidget->addItems(programList);
        model->setStringList(programList);
        break;
    }

    case 1: // 02_Dock_Undock
    {
        programList.clear();
        programList << "printf('Connecting to Create3 robot...');"
                    << "create3_connect_manual('192.168.125.1', 50051);"
                    << "printf('Connected!');"
                    << "printf('Undocking...');"
                    << "create3_undock();"
                    << "create3_wait();"
                    << "printf('Done!');";
        qDebug() << "Program List: " << programList;

        ui->listWidget->addItems(programList);
        model->setStringList(programList);
        break;
    }
    case 2: // 03_Move_Velocity
    {

        programList
            << "printf('Connecting to Create3 robot...');"
            << "create3_connect_manual('192.168.125.1', 50051);"
            << "printf('Connected!');"
            << "printf('Moving forward at 0.5 m/s for 1 second...');"
            << "//Move forward at 0.5 m/s for 1 second"
            << "create3_velocity_set_components(0.5, 0);"
            << "//Wait for 1 second"
            << "usleep(1000000);"
            << "//Stop"
            << "create3_velocity_set_components(0.0, 0.0);"
            << "printf('Done!');";
        qDebug() << "Program List: " << programList;
        ui->listWidget->addItems(programList);
        model->setStringList(programList);
        ui->listWidget->item(4)->setForeground(QColor(0, 128, 0));
        ui->listWidget->item(6)->setForeground(QColor(0, 128, 0));
        ui->listWidget->item(8)->setForeground(QColor(0, 128, 0));

        break;
    }
    case 3: // 04_Rotate
        programList
            << "printf('Connecting to Create3 robot...');"
            << "create3_connect_manual('192.168.125.1', 50051);"
            << "printf('Connected!');"
            << "printf('Rotate at 0.5 m/s for 1 second...');"
            << "//Rotate at 90 deg/s (PI/2 rad/s) to 90 degrees (PI/2 radians)"
            << "create3_rotate_degrees(90, 90);"
            << "create3_rotate_radians(1.57, 1.57);"
            << "//Wait for 1 second"
            << "create3_wait();"
            << "printf('Done!');";
        ui->listWidget->addItems(programList);
        model->setStringList(programList);
        ui->listWidget->item(4)->setForeground(QColor(0, 128, 0));
        ui->listWidget->item(7)->setForeground(QColor(0, 128, 0));

        break;
    case 4: // 05_Arc
        programList
            << "printf('Connecting to Create3 robot...');"
            << "create3_connect_manual('192.168.125.1', 50051);"
            << "printf('Connected!');"
            << "printf('Rotate at 0.5 m/s for 1 second...');"
            << "//Arc of 90 deg (PI/2 rad) with radius of 0.5 m"
            << "create3_drive_arc_degrees(0.5, 90, 0.306);"
            << "create3_drive_arc_radians(0.5, -1.57, 0.460);"
            << "//Wait for 1 second"
            << "create3_wait();"
            << "printf('Done!');";
        ui->listWidget->addItems(programList);
        model->setStringList(programList);
        ui->listWidget->item(4)->setForeground(QColor(0, 128, 0));
        ui->listWidget->item(7)->setForeground(QColor(0, 128, 0));
        break;
    case 5: // 06_led_test
        programList
            << "printf('Connecting to Create3 robot...');"
            << "create3_connect_manual('192.168.125.1', 50051);"
            << "printf('Connected!');"
            << "printf('Blinking lights...');"
            << "//Blink the lights green for 5 seconds."
            << "Create3LedColor green;"
            << "green.r = 0;"
            << "green.g = 255;"
            << "green.b = 0;"
            << "Create3Lightring lightring;"
            << "lightring.led0 = green;"
            << "lightring.led1 = green;"
            << "lightring.led2 = green;"
            << "lightring.led3 = green;"
            << "lightring.led4 = green;"
            << "lightring.led5 = green;"
            << "// Blink the lights green for 5 seconds."
            << "create3_led_animation(Create3BlinkLights, lightring, 5.0);"
            << "//Wait for 1 second"
            << "create3_wait();"
            << "printf('Done!');";
        ui->listWidget->addItems(programList);
        model->setStringList(programList);
        ui->listWidget->item(4)->setForeground(QColor(0, 128, 0));
        ui->listWidget->item(16)->setForeground(QColor(0, 128, 0));
        ui->listWidget->item(18)->setForeground(QColor(0, 128, 0));
        break;
    case 6: // 07_sensors
        programList
            << "printf('Connecting to Create3 robot...');"
            << "create3_connect_manual('192.168.125.1', 50051);"
            << "printf('Connected!');"
            << "printf('Sensor Values:');"
            << "printf('  Left Side Cliff: %d', create3_sensor_cliff(0));"
            << "printf('  Left Front Cliff: %d', create3_sensor_cliff(1));"
            << "printf('  Right Front Cliff: %d', create3_sensor_cliff(2));"
            << "printf('  Right Side Cliff: %d', create3_sensor_cliff(3));"
            << "printf('  Left Side IR: %d', create3_sensor_ir(0));"
            << "printf('  Left IR: %d', create3_sensor_ir(1));"
            << "printf('  Left Front IR: %d', create3_sensor_ir(2));"
            << "printf('  Left Center IR: %d', create3_sensor_ir(3));"
            << "printf('  Right Center IR: %d', create3_sensor_ir(4));"
            << "printf('  Right Front IR: %d', create3_sensor_ir(5));"
            << "printf('  Right IR: %d', create3_sensor_ir(6));"
            << "printf('  Left Bumper: %d', create3_sensor_bump(0));"
            << "printf('  Left Front Bumper: %d', create3_sensor_bump(1));"
            << "printf('  Center Front Bumper: %d', create3_sensor_bump(2));"
            << "printf('  Right Front Bumper: %d', create3_sensor_bump(3));"
            << "printf('  Right Bumper: %d', create3_sensor_bump(4));"
            << "while (create3_sensor_bump(3) == 0)"
            << "{"
            << "    if (create3_sensor_bump(3) == 1)"
            << "    {"
            << "        create3_velocity_set_components(0.0, 0.0);"
            << "        break;"
            << "    }"
            << "    create3_velocity_set_components(0.15, 0.0);"
            << "}"
            << "printf('Done!');";
        ui->listWidget->addItems(programList);
        model->setStringList(programList);

        break;
    default:
        break;
    }
}

void Create3Widget::run()
{

    exampleIndex = ui->exampleComboBox->currentIndex();
    switch (exampleIndex)
    {

    case 0:
    { // 01_Connect
        try
        {
            printf("Connecting to Create3 robot...\n");
            create3_connect_manual("192.168.125.1", 50051);
            printf("Connected!\n");
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
        break;
    }

    case 1: // 02_Dock_Undock
    {

        try
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
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }

        break;
    }

    case 2: // 03_Move_Velocity
    {
        try
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
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
        break;
    }

    case 3: // 04_Rotate
    {
        try
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
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }

        break;
    }

    case 4: // 05_Arc
    {
        try
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
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }

        break;
    }

    case 5: // 06_led_test
    {
        try
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
        }
        // catch (const std::exception &e)
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }

        break;
    }
    case 6: // 07_sensors
    {
        try
        {
            printf("Connecting to Create3 robot...\n");

            create3_connect_manual("192.168.125.1", 50051);

            printf("Connected!\n");

            printf("Sensor Values:\n");
            printf("  Left Side Cliff: %d\n", create3_sensor_cliff(0));
            printf("  Left Front Cliff: %d\n", create3_sensor_cliff(1));
            printf("  Right Front Cliff: %d\n", create3_sensor_cliff(2));
            printf("  Right Side Cliff: %d\n", create3_sensor_cliff(3));
            printf("\n");
            printf("  Left Side IR: %d\n", create3_sensor_ir(0));
            printf("  Left IR: %d\n", create3_sensor_ir(1));
            printf("  Left Front IR: %d\n", create3_sensor_ir(2));
            printf("  Left Center IR: %d\n", create3_sensor_ir(3));
            printf("  Right Center IR: %d\n", create3_sensor_ir(4));
            printf("  Right Front IR: %d\n", create3_sensor_ir(5));
            printf("  Right IR: %d\n", create3_sensor_ir(6));

            printf("  Left Bumper: %d\n", create3_sensor_bump(0));
            printf("  Left Front Bumper: %d\n", create3_sensor_bump(1));
            printf("  Center Front Bumper: %d\n", create3_sensor_bump(2));
            printf("  Right Front Bumper: %d\n", create3_sensor_bump(3));
            printf("  Right Bumper: %d\n", create3_sensor_bump(4));

            while (create3_sensor_bump(3) == 0)
            {
                if (create3_sensor_bump(3) == 1)
                {
                    create3_velocity_set_components(0.0, 0.0);
                    break;
                }
                create3_velocity_set_components(0.15, 0.0);
            }

            printf("Done!\n");
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    default:
        break;
    }
}
