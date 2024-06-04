#include "TelloWidget.h"
#include "ui_TelloWidget.h"
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

#include <kipr/tello/Tello.h>

TelloWidget::TelloWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::TelloWidget)
{
    ui->setupUi(this);
    performStandardSetup(tr("Tello"), false);
    setStyleSheet("QScrollBar:vertical {border: 2px solid grey;background:grey ;width: 100px; margin: 22px 0 22px 0; } QScrollBar::handle:vertical { border: 2px solid grey;background: white; min-height: 20px; } QScrollBar::add-line:vertical { border: 2px solid grey;background: #32CC99; height: 20px; subcontrol-position: bottom; subcontrol-origin: margin; } QScrollBar::sub-line:vertical {border: 2px solid grey; background: #32CC99; height: 20px; subcontrol-position: top; subcontrol-origin: margin; } QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { border: 2px solid grey;width: 3px; height: 3px; background: white; }QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {border: 2px solid grey; background: none;}");
    connect(ui->TelloConnect, SIGNAL(clicked()), SLOT(telloConnect()));


}

TelloWidget::~TelloWidget()
{
    delete ui;
}

int TelloWidget::telloConnect()
{

    // int result;
    // char buf[TELLO_BUFSIZE];
    // int len;
    // int n;
    // struct tello_ssid *tellos;
    // struct tello_info tello;
    // int send_result;

    
    // if (wpa_sup_connect() == -1)
    // {
    //     qDebug() << "failed to open wpa_supplicant\n";

    //     return -1;
    // }
    // else
    //     qDebug() << "wpa_supplicant opened\n";

    // tellos = tellos_find();

    // n = 0;
    // qDebug() << "List of Tellos\n";

    // while (tellos[n].ssid[0] != '\0')
    // {
    //     qDebug() << "Tellos: %s\n"
    //              << tellos[n].ssid;
    //     // printf("Tellos: %s\n", tellos[n].ssid);
    //     n++;
    // }
    // fflush(NULL);

    // // for this example select only the first Tello in the list

    // qDebug() << "Tellos[]: " << tellos[0].ssid;
    // tello_connect(&tello, tellos[0].ssid);


}
