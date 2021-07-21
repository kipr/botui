#include "FactoryWidget.h"
#include "ui_FactoryWidget.h"

#include <QApplication>
#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include <QFileInfo>

#include "Device.h"
#include "NumpadDialog.h"
#include "RootController.h"

FactoryWidget::FactoryWidget(Device *device, QQuickWidget *parent)
	: QQuickWidget(parent),
	m_device(device),
	m_serialNumpad(new NumpadDialog(tr("Serial Number"), NumpadDialog::Integer)),
	ui(new Ui::FactoryWidget)
{

        ui->updateConsole->setVisible(false);
	ui->setupUi(this);
	
	ui->serialNumber->setInputProvider(m_serialNumpad);
	connect(ui->confirm, SIGNAL(clicked()), SLOT(confirm()));
        connect(ui->reflash, SIGNAL(clicked()), SLOT(reflash()));
        connect(ui->experimental, SIGNAL(clicked()), SLOT(experimental()));
}

FactoryWidget::~FactoryWidget()
{
	delete m_serialNumpad;
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
=======
	RootController::ref().dismissWidget();
}
