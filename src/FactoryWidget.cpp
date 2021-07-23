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
        ui->console->setVisible(false);

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

    QProcess backup_process;
    backup_process.startDetached("/bin/sh", QStringList() << "/home/pi/wallaby_flash.sh " << ui->serialOne << " " << ui->serialTwo << " " << ui->serialThree << " " << ui->serialFour);
    backup_process.waitForFinished(); // sets current thread to sleep
    ui->console->insertPlainText("Serial Number Set to " + ui->serialOne + " " + ui->serialTwo + " " + ui->serialThree + " " + ui->serialFour);

}

void FactoryWidget::reflash()
{

    QProcess backup_process;
    backup_process.startDetached("/bin/sh", QStringList()<< "/home/pi/*_flash.sh"); //*_flash is to account for when wallaby_flash gets updated to wombat_flash
    backup_process.waitForFinished(); // sets current thread to sleep
    ui->console->insertPlainText("Flash Complete");

}

void FactoryWidget::experimental()
{

    QProcess backup_process;
    backup_process.startDetached("/bin/sh", QStringList()<< "/home/pi/getExperimental.sh");
    backup_process.waitForFinished(); // sets current thread to sleep
    ui->console->insertPlainText("Experimental Build Installed");

}
