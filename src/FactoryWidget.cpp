#include "FactoryWidget.h"
#include "ui_FactoryWidget.h"

#include <QApplication>
#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include <QFileInfo>
#include <iostream>

#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "Options.h"
#include "NotYetImplementedDialog.h"


FactoryWidget::FactoryWidget(Device *device, QWidget *parent)
        : StandardWidget(device, parent),
        ui(new Ui::FactoryWidget)
{
        std::cout << "Test 1" << std::endl;
        ui->flashConsole->setVisible(false);
        std::cout << "Test 2" << std::endl;
	ui->setupUi(this);
        std::cout << "Test 3" << std::endl;
        //ui->serialNumber->setInputProvider(serialNumber);
	connect(ui->confirm, SIGNAL(clicked()), SLOT(confirm()));
        connect(ui->reflash, SIGNAL(clicked()), SLOT(reflash()));
        connect(ui->experimental, SIGNAL(clicked()), SLOT(experimental()));

}

FactoryWidget::~FactoryWidget()
{
        //delete serialNumber;
	delete ui;
}

void FactoryWidget::confirm()
{
    /*
     * NOT YET IMPLEMENTED
    QProcess backup_process;
    backup_process.startDetached("/bin/sh", QStringList()<< "/home/pi/wallaby_flash.sh " << ui->serialNumber);
    backup_process.waitForFinished(); // sets current thread to sleep
    ui->updateConsole->insertPlainText("STM32 has been Flashed");
    QMessageBox::warning(this, "STM32 has been Flashed", "STM32 has been Flashed");
    */
}

void FactoryWidget::reflash()
{

    QProcess backup_process;
    backup_process.startDetached("/bin/sh", QStringList()<< "/home/pi/*_flash.sh");
    backup_process.waitForFinished(); // sets current thread to sleep
    ui->updateConsole->insertPlainText("Flash Complete");
    QMessageBox::warning(this, "Flash Complete", "Flash Complete");

}

void FactoryWidget::experimental()
{
    /*
     * NOT YET IMPLEMENTED
    QProcess backup_process;
    backup_process.startDetached("/bin/sh", QStringList()<< "/home/pi/got2/Backup/backup.sh");
    backup_process.waitForFinished(); // sets current thread to sleep
    ui->updateConsole->insertPlainText("Backup Complete");
    QMessageBox::warning(this, "Backup complete", "Backup complete");
    */
}
