#include "FactoryWidget.h"
#include "ui_FactoryWidget.h"
#include "SettingsProvider.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "Calibrate.h"
#include "Options.h"
#include "NotYetImplementedDialog.h"
#include <QString>
#include <QProcess>


#include <QDebug>

FactoryWidget::FactoryWidget(Device *device, QWidget *parent)
        : StandardWidget(device, parent),
        ui(new Ui::FactoryWidget)
{
        ui->setupUi(this);
        performStandardSetup(tr("Factory"));


        connect(ui->confirm, SIGNAL(clicked()), SLOT(confirm()));
        connect(ui->reflash, SIGNAL(clicked()), SLOT(reflash()));
        connect(ui->experimental, SIGNAL(clicked()), SLOT(experimental()));

}

FactoryWidget::~FactoryWidget()
{
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
    //ui->flashConsole->insertPlainText("Flash Complete");

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
