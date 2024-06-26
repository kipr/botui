// #include "Create3ExampleWidget.h"
// #include "ui_Create3ExampleWidget.h"
// #include "Create3SensorListWidget.h"
// #include <kipr/create3/client/client.h>
// #include <unistd.h>
// #include "NumpadDialog.h"
// #include <QDebug>
// #include <stdio.h>
// #include <iostream>
// #include <QRegularExpression>
// #include <QProcess>
// #include <QHeaderView>
// #include <cmath>
// #include <QTimer>
// #include <QPainter>
// #include <QItemDelegate>
// #include "RootController.h"
// #include <QStringListModel>
// #include <QStandardItemModel>
// #include <QStandardItem>
// #include <QColor>
// int exampleIndex;
// QStringList programList;
// // QStandardItemModel model(4,4);
// Create3ExampleWidget::Create3ExampleWidget(Device *device, QWidget *parent)
//     : StandardWidget(device, parent),
//       ui(new Ui::Create3ExampleWidget)
// {
//     ui->setupUi(this);
//     performStandardSetup(tr("Create 3 Examples"), false);
//     connect(ui->exampleComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));

//     connect(ui->exampleRunButton, SIGNAL(clicked()), SLOT(run()));

//     // Create model
//     model = new QStringListModel(this);
//     ui->listWidget->clear();
//     programList.clear();
//     programList << "printf('Connecting to Create3 robot...');"
//                 << "create3_connect_manual('192.168.125.1', 50051);"
//                 << "printf('Connected!');";

//     ui->listWidget->addItems(programList);
//     model->setStringList(programList);
//     qDebug() << "Program List: " << programList;
//     ui->listWidget->setSpacing(6);
// }

// Create3ExampleWidget::~Create3ExampleWidget()
// {
//     delete ui;
// }

// int Create3ExampleWidget::isConnected()
// {
//     int connected = 0;
//     try
//     {
//         connected = create3_is_connected();
//         qDebug() << "Create connected? " << connected;
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << e.what() << '\n';
//     }
//     return connected;
// }

// int Create3ExampleWidget::create3Connect()
// {
//     int connected = 0;
//     try
//     {
//         connected = create3_connect_manual("192.168.125.1", 50051);
//         qDebug() << "Create connected? " << connected;
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << e.what() << '\n';
//     }
//     return connected;
// }
// void Create3ExampleWidget::run()
// {
//     exampleIndex = ui->exampleComboBox->currentIndex();
//     switch (exampleIndex)
//     {

//     case 0:
//     { // 01_Connect
//         try
//         {
//             printf("Connecting to Create3 robot...\n");
//             create3_connect_manual("192.168.125.1", 50051);
//             printf("Connected!\n");
//         }
//         catch (const std::exception &e)
//         {
//             std::cerr << e.what() << '\n';
//         }
//         break;
//     }

//     case 1: // 02_Dock_Undock
//     {

//         try
//         {
//             printf("Connecting to Create3 robot...\n");

//             create3_connect_manual("192.168.125.1", 50051);

//             printf("Connected!\n");

//             printf("Undocking...\n");
//             // Move forward at 0.5 m/s for 1 second
//             create3_undock();

//             create3_wait();

//             printf("Docking...\n");

//             create3_dock();

//             create3_wait();

//             printf("Done!\n");
//         }
//         catch (const std::exception &e)
//         {
//             std::cerr << e.what() << '\n';
//         }

//         break;
//     }

//     case 2: // 03_Move_Velocity
//     {
//         try
//         {
//             printf("Connecting to Create3 robot...\n");

//             create3_connect_manual("192.168.125.1", 50051);

//             printf("Connected!\n");

//             printf("Moving forward at 0.5 m/s for 1 second...\n");

//             // Move forward at 0.5 m/s for 1 second
//             create3_velocity_set_components(0.5, 0);

//             // Wait for 1 second
//             usleep(1000000);

//             // Stop
//             create3_velocity_set_components(0.0, 0.0);

//             printf("Done!\n");
//         }
//         catch (const std::exception &e)
//         {
//             std::cerr << e.what() << '\n';
//         }
//         break;
//     }

//     case 3: // 04_Rotate
//     {
//         try
//         {
//             printf("Connecting to Create3 robot...\n");

//             create3_connect_manual("192.168.125.1", 50051);

//             printf("Connected!\n");

//             printf("Rotate at 0.5 m/s for 1 second...\n");

//             // Rotate at 90 deg/s (PI/2 rad/s) to 90 degrees (PI/2 radians)
//             create3_rotate_degrees(90, 90);
//             create3_rotate_radians(1.57, 1.57);

//             // Wait for 1 second
//             create3_wait();

//             printf("Done!\n");
//         }
//         catch (const std::exception &e)
//         {
//             std::cerr << e.what() << '\n';
//         }

//         break;
//     }

//     case 4: // 05_Arc
//     {
//         try
//         {
//             printf("Connecting to Create3 robot...\n");

//             create3_connect_manual("192.168.125.1", 50051);

//             printf("Connected!\n");

//             printf("Rotate at 0.5 m/s for 1 second...\n");

//             // Arc of 90 deg (PI/2 rad) with radius of 0.5 m
//             create3_drive_arc_degrees(0.5, 90, 0.306);
//             create3_drive_arc_radians(0.5, -1.57, 0.460);

//             // Wait for 1 second
//             create3_wait();

//             printf("Done!\n");
//         }
//         catch (const std::exception &e)
//         {
//             std::cerr << e.what() << '\n';
//         }

//         break;
//     }

//     case 5: // 06_led_test
//     {
//         try
//         {
//             printf("Connecting to Create3 robot...\n");

//             create3_connect_manual("192.168.125.1", 50051);

//             printf("Connected!\n");

//             printf("Blinking lights...\n");
//             // Blink the lights green for 5 seconds.

//             Create3LedColor green;
//             green.r = 0;
//             green.g = 255;
//             green.b = 0;

//             Create3Lightring lightring;
//             lightring.led0 = green;
//             lightring.led1 = green;
//             lightring.led2 = green;
//             lightring.led3 = green;
//             lightring.led4 = green;
//             lightring.led5 = green;

//             create3_led_animation(Create3BlinkLights, lightring, 5.0); // Blink the lights green for 5 seconds.

//             // Wait for 1 second
//             create3_wait();

//             printf("Done!\n");
//         }
//         catch (const std::exception &e)
//         {
//             std::cerr << e.what() << '\n';
//         }

//         break;
//     }
//     case 6: // 07_sensors
//     {
//         try
//         {
//             printf("Connecting to Create3 robot...\n");

//             create3_connect_manual("192.168.125.1", 50051);

//             printf("Connected!\n");

//             printf("Sensor Values:\n");
//             printf("  Left Side Cliff: %d\n", create3_sensor_cliff(0));
//             printf("  Left Front Cliff: %d\n", create3_sensor_cliff(1));
//             printf("  Right Front Cliff: %d\n", create3_sensor_cliff(2));
//             printf("  Right Side Cliff: %d\n", create3_sensor_cliff(3));
//             printf("\n");
//             printf("  Left Side IR: %d\n", create3_sensor_ir(0));
//             printf("  Left IR: %d\n", create3_sensor_ir(1));
//             printf("  Left Front IR: %d\n", create3_sensor_ir(2));
//             printf("  Left Center IR: %d\n", create3_sensor_ir(3));
//             printf("  Right Center IR: %d\n", create3_sensor_ir(4));
//             printf("  Right Front IR: %d\n", create3_sensor_ir(5));
//             printf("  Right IR: %d\n", create3_sensor_ir(6));

//             printf("  Left Bumper: %d\n", create3_sensor_bump(0));
//             printf("  Left Front Bumper: %d\n", create3_sensor_bump(1));
//             printf("  Center Front Bumper: %d\n", create3_sensor_bump(2));
//             printf("  Right Front Bumper: %d\n", create3_sensor_bump(3));
//             printf("  Right Bumper: %d\n", create3_sensor_bump(4));

//             while (create3_sensor_bump(3) == 0)
//             {
//                 if (create3_sensor_bump(3) == 1)
//                 {
//                     create3_velocity_set_components(0.0, 0.0);
//                     break;
//                 }
//                 create3_velocity_set_components(0.15, 0.0);
//             }

//             printf("Done!\n");
//         }
//         catch (const std::exception &e)
//         {
//             std::cerr << e.what() << '\n';
//         }
//     }

//     default:
//         break;
//     }
// }
// void Create3ExampleWidget::indexChanged(int index)
// {

//     programList.clear();
//     ui->listWidget->clear();
//     ui->listWidget->addItems(programList);
//     model->setStringList(programList);
//     switch (index)
//     {
//     case 0: // 01_Connect
//     {

//         programList << "printf('Connecting to Create3 robot...');"
//                     << "create3_connect_manual('192.168.125.1', 50051);"
//                     << "printf('Connected!');";
//         qDebug() << "Program List: " << programList;
//         ui->listWidget->addItems(programList);
//         model->setStringList(programList);
//         break;
//     }

//     case 1: // 02_Dock_Undock
//     {
//         programList.clear();
//         programList << "printf('Connecting to Create3 robot...');"
//                     << "create3_connect_manual('192.168.125.1', 50051);"
//                     << "printf('Connected!');"
//                     << "printf('Undocking...');"
//                     << "create3_undock();"
//                     << "create3_wait();"
//                     << "printf('Done!');";
//         qDebug() << "Program List: " << programList;

//         ui->listWidget->addItems(programList);
//         model->setStringList(programList);
//         break;
//     }
//     case 2: // 03_Move_Velocity
//     {

//         programList
//             << "printf('Connecting to Create3 robot...');"
//             << "create3_connect_manual('192.168.125.1', 50051);"
//             << "printf('Connected!');"
//             << "printf('Moving forward at 0.5 m/s for 1 second...');"
//             << "//Move forward at 0.5 m/s for 1 second"
//             << "create3_velocity_set_components(0.5, 0);"
//             << "//Wait for 1 second"
//             << "usleep(1000000);"
//             << "//Stop"
//             << "create3_velocity_set_components(0.0, 0.0);"
//             << "printf('Done!');";
//         qDebug() << "Program List: " << programList;
//         ui->listWidget->addItems(programList);
//         model->setStringList(programList);
//         ui->listWidget->item(4)->setForeground(QColor(0, 128, 0));
//         ui->listWidget->item(6)->setForeground(QColor(0, 128, 0));
//         ui->listWidget->item(8)->setForeground(QColor(0, 128, 0));

//         break;
//     }
//     case 3: // 04_Rotate
//         programList
//             << "printf('Connecting to Create3 robot...');"
//             << "create3_connect_manual('192.168.125.1', 50051);"
//             << "printf('Connected!');"
//             << "printf('Rotate at 0.5 m/s for 1 second...');"
//             << "//Rotate at 90 deg/s (PI/2 rad/s) to 90 degrees (PI/2 radians)"
//             << "create3_rotate_degrees(90, 90);"
//             << "create3_rotate_radians(1.57, 1.57);"
//             << "//Wait for 1 second"
//             << "create3_wait();"
//             << "printf('Done!');";
//         ui->listWidget->addItems(programList);
//         model->setStringList(programList);
//         ui->listWidget->item(4)->setForeground(QColor(0, 128, 0));
//         ui->listWidget->item(7)->setForeground(QColor(0, 128, 0));

//         break;
//     case 4: // 05_Arc
//         programList
//             << "printf('Connecting to Create3 robot...');"
//             << "create3_connect_manual('192.168.125.1', 50051);"
//             << "printf('Connected!');"
//             << "printf('Rotate at 0.5 m/s for 1 second...');"
//             << "//Arc of 90 deg (PI/2 rad) with radius of 0.5 m"
//             << "create3_drive_arc_degrees(0.5, 90, 0.306);"
//             << "create3_drive_arc_radians(0.5, -1.57, 0.460);"
//             << "//Wait for 1 second"
//             << "create3_wait();"
//             << "printf('Done!');";
//         ui->listWidget->addItems(programList);
//         model->setStringList(programList);
//         ui->listWidget->item(4)->setForeground(QColor(0, 128, 0));
//         ui->listWidget->item(7)->setForeground(QColor(0, 128, 0));
//         break;
//     case 5: // 06_led_test
//         programList
//             << "printf('Connecting to Create3 robot...');"
//             << "create3_connect_manual('192.168.125.1', 50051);"
//             << "printf('Connected!');"
//             << "printf('Blinking lights...');"
//             << "//Blink the lights green for 5 seconds."
//             << "Create3LedColor green;"
//             << "green.r = 0;"
//             << "green.g = 255;"
//             << "green.b = 0;"
//             << "Create3Lightring lightring;"
//             << "lightring.led0 = green;"
//             << "lightring.led1 = green;"
//             << "lightring.led2 = green;"
//             << "lightring.led3 = green;"
//             << "lightring.led4 = green;"
//             << "lightring.led5 = green;"
//             << "// Blink the lights green for 5 seconds."
//             << "create3_led_animation(Create3BlinkLights, lightring, 5.0);"
//             << "//Wait for 1 second"
//             << "create3_wait();"
//             << "printf('Done!');";
//         ui->listWidget->addItems(programList);
//         model->setStringList(programList);
//         ui->listWidget->item(4)->setForeground(QColor(0, 128, 0));
//         ui->listWidget->item(16)->setForeground(QColor(0, 128, 0));
//         ui->listWidget->item(18)->setForeground(QColor(0, 128, 0));
//         break;
//     case 6: // 07_sensors
//          programList
//             << "printf('Connecting to Create3 robot...');"
//             << "create3_connect_manual('192.168.125.1', 50051);"
//             << "printf('Connected!');"
//             << "printf('Sensor Values:');"
//             << "printf('  Left Side Cliff: %d', create3_sensor_cliff(0));"
//             << "printf('  Left Front Cliff: %d', create3_sensor_cliff(1));"
//             << "printf('  Right Front Cliff: %d', create3_sensor_cliff(2));"
//             << "printf('  Right Side Cliff: %d', create3_sensor_cliff(3));"
//             << "printf('  Left Side IR: %d', create3_sensor_ir(0));"
//             << "printf('  Left IR: %d', create3_sensor_ir(1));"
//             << "printf('  Left Front IR: %d', create3_sensor_ir(2));"
//             << "printf('  Left Center IR: %d', create3_sensor_ir(3));"
//             << "printf('  Right Center IR: %d', create3_sensor_ir(4));"
//             << "printf('  Right Front IR: %d', create3_sensor_ir(5));"
//             << "printf('  Right IR: %d', create3_sensor_ir(6));"
//             << "printf('  Left Bumper: %d', create3_sensor_bump(0));"
//             << "printf('  Left Front Bumper: %d', create3_sensor_bump(1));"
//             << "printf('  Center Front Bumper: %d', create3_sensor_bump(2));"
//             << "printf('  Right Front Bumper: %d', create3_sensor_bump(3));"
//             << "printf('  Right Bumper: %d', create3_sensor_bump(4));"
//             << "while (create3_sensor_bump(3) == 0)"
//             << "{"
//             << "    if (create3_sensor_bump(3) == 1)"
//             << "    {"
//             << "        create3_velocity_set_components(0.0, 0.0);"
//             << "        break;"
//             << "    }"
//             << "    create3_velocity_set_components(0.15, 0.0);"
//             << "}"
//             << "printf('Done!');";
//         ui->listWidget->addItems(programList);
//         model->setStringList(programList);

//         break;
//     default:
//         break;
//     }
// }
