#include "BackupWidget.h"
#include "ui_BackupWidget.h"

#include <QApplication>
#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include <QFileInfo>

#include "MenuBar.h"
#include "StatusBar.h"
#include "RootController.h"
#include "SettingsWidget.h"
#include "AboutWidget.h"
#include "UnderConstructionWidget.h"
#include "MotorsSensorsWidget.h"
#include "ProgramsWidget.h"
#include "FileManagerWidget.h"
#include "Device.h"
#include "ProgramSelectionWidget.h"
#include "Program.h"
#include "UiStandards.h"
#include "LockScreen.h"
#include "SystemUtils.h"


BackupWidget::BackupWidget(Device *device, QWidget *widget)
	: StandardWidget(device, widget),
	ui(new Ui::BackupWidget)
{
	ui->setupUi(this);
	performStandardSetup("Backup");
	
        ui->updateConsole->setVisible(false);
	
        connect(ui->backupoption, SIGNAL(clicked()), SLOT(backupoption()));
        connect(ui->restore, SIGNAL(clicked()), SLOT(restore()));
        //connect(ui->cleardrive, SIGNAL(clicked()), SLOT(cleardrive()));
}

BackupWidget::~BackupWidget()
{
	delete ui;
}

void BackupWidget::backupoption()
{
        QProcess backup_process;
        backup_process.startDetached("/bin/sh", QStringList()<< "/home/pi/got2/Backup/backup.sh");
        backup_process.waitForFinished(); // sets current thread to sleep and waits for Backup end
        ui->updateConsole->insertPlainText("Backup Complete");
        QMessageBox::warning(this, "Backup complete", "Backup complete");
}

void BackupWidget::restore()
{
    QProcess restore_process;
    restore_process.startDetached("/bin/sh", QStringList()<< "/home/pi/got2/Backup/restore.sh");
    restore_process.waitForFinished(); // sets current thread to sleep and waits for Restore end
    ui->updateConsole->insertPlainText("Restore Complete");
    QMessageBox::warning(this, "Restore complete", "Restore Complete");
}

/*void BackupWidget::cleardrive()
{
    QProcess clear_process;
    clear_process.startDetached("/bin/sh", QStringList()<< "/home/pi/got2/Backup/ClearDrive.sh");
    clear_process.waitForFinished(); // sets current thread to sleep and waits for Clear end
    ui->updateConsole->insertPlainText("Backup Complete");
    QMessageBox::warning(this, "complete", "Clear Completed");
}*/
