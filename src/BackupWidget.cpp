#include "BackupWidget.h"
#include "ui_BackupWidget.h"

#include <QApplication>
#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>

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

QStringList errorFiles;

BackupWidget::BackupWidget(Device *device, QWidget *widget)
    : StandardWidget(device, widget),
      ui(new Ui::BackupWidget)
{
    ui->setupUi(this);
    performStandardSetup("Backup");

    ui->backupConsole->setVisible(true);

    connect(ui->backupoption, SIGNAL(clicked()), SLOT(backupoption()));
    connect(ui->restore, SIGNAL(clicked()), SLOT(restore()));
    // connect(ui->cleardrive, SIGNAL(clicked()), SLOT(cleardrive()));
}

BackupWidget::~BackupWidget()
{
    delete ui;
}
const QDir BackupWidget::mountDir = QDir("/mnt");
void BackupWidget::backupoption()
{

    // Mount USB drive
    if (!this->mountUsb("/dev/sda1", BackupWidget::mountDir) && !this->mountUsb("/dev/sdb1", BackupWidget::mountDir))
        QMessageBox::warning(this, "USB not found", "Failed to mount USB device\n Please insert USB device and try again");
    else
    {
        // Verify with user that they want to do the backup
        if (QMessageBox::question(this, "Backup?",
                                  QString("Are you sure you want to backup your programs?"),
                                  QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
            return;

        // Disable buttons
        ui->backupoption->setEnabled(false);
        ui->restore->setEnabled(false);
        StandardWidget::disableMenuBar();

        ui->BackupRestoreLabel->setText("Backup in progress...");
        backup_process = new QProcess();

        // Capture output and error from backup.sh
        connect(backup_process, &QProcess::readyReadStandardOutput, this, &BackupWidget::handleStandardOutput);
        connect(backup_process, &QProcess::readyReadStandardError, this, &BackupWidget::handleStandardError);

        QString command = QString("/home/kipr/wombat-os/Backup/backup.sh");
        backup_process->start("sh", QStringList() << "-c" << command);
        ui->backupConsole->setProcess(backup_process);
        connect(backup_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(updateFinished(int, QProcess::ExitStatus)));
    }
}

bool BackupWidget::isAlreadyMounted(const QString &device, const QString &mountPoint)
{
    QFile file("/proc/mounts");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open /proc/mounts to check mounted devices.";
        return false;
    }
    QString normalizedMountPoint = QDir::cleanPath(mountPoint);
    qDebug() << "Checking if device:" << device << "is mounted at:" << normalizedMountPoint;

    QTextStream in(&file);
    qDebug() << "in[0]: " << in.readLine();
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList parts = line.split(" ");
        if (parts.size() >= 2)
        {
            QString mountedDevice = parts[0];
            QString mountedPath = QDir::cleanPath(parts[1]);
            qDebug() << "Found mounted device:" << mountedDevice << "at path:" << mountedPath;

            if (mountedDevice == device && mountedPath == normalizedMountPoint)
            {
                qDebug() << "Device is already mounted.";
                return true; // Device is already mounted at the target path
            }
        }
    }
    qDebug() << "Device is not mounted.";
    return false;
}

bool BackupWidget::mountUsb(const QString device, const QDir dir)
{
    //   QProcess proc;
    //   proc.start("mount", QStringList() << device << dir.absolutePath());
    //   return proc.waitForFinished(5000) && proc.exitCode() == 0;

    // QProcess proc;
    // proc.start("sudo", QStringList() << "mount" << device << dir.absolutePath());
    // if (!proc.waitForFinished(5000))
    // {
    //     qDebug() << "Mount process timed out for device:" << device;
    //     return false;
    // }
    // if (proc.exitCode() != 0)
    // {
    //     qDebug() << "Mount failed for device:" << device
    //              << ", Error:" << proc.readAllStandardError();
    //     return false;
    // }
    // return true;

    // Check if already mounted
    if (isAlreadyMounted(device, dir.absolutePath()))
    {
        qDebug() << "Device already mounted:" << device << "at" << dir.absolutePath();
        return true;
    }
    // Proceed with mounting
    QProcess proc;
    proc.start("sudo", QStringList() << "mount" << device << dir.absolutePath());
    if (!proc.waitForFinished(5000))
    {
        qDebug() << "Mount process timed out for device:" << device;
        return false;
    }
    if (proc.exitCode() != 0)
    {
        qDebug() << "Mount failed for device:" << device
                 << ", Error:" << proc.readAllStandardError();
        return false;
    }

    qDebug() << "Device successfully mounted:" << device << "at" << dir.absolutePath();
    return true;
}

bool BackupWidget::unmountUsb(const QString device)
{
    QProcess proc;
    proc.start("umount", QStringList() << device);
    return proc.waitForFinished(5000) && proc.exitCode() == 0;
}

void BackupWidget::updateFinished(int, QProcess::ExitStatus exitStatus)
{
    QMessageBox::warning(this, "Backup complete", "Backup complete");
    StandardWidget::enableMenuBar();

    // Check to see if the update failed
    if (backup_process->exitStatus() != QProcess::NormalExit)
    {
        ui->backupConsole->insertPlainText("\n Update Failed (Crashed): \n The update script has crashed with an error. \n Contact KIPR tech support for assistance if the problem persists \n");
        ui->backupConsole->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
    }

    ui->backupConsole->append("-------------------------");
    ui->backupConsole->append("-------------------------");
    ui->backupConsole->append("\n");
    ui->backupConsole->append("WARNING - The following files were not backed up:\n");
    // Log only the file paths from errorFiles (if any)
    if (!errorFiles.isEmpty())
    {
        qDebug() << "Backup error files: ";
        foreach (const QString &filePath, errorFiles)
        {
            qDebug() << filePath; // Logs each file path individually
            ui->backupConsole->append(filePath);
            ui->backupConsole->append(""); // Add a new line after each file path
        }
    }

    // Cleanup process
    ui->backupConsole->setProcess(0);
    delete backup_process;

    // Re-enable buttons
    ui->backupoption->setEnabled(true);
    ui->restore->setEnabled(true);
}

void BackupWidget::restoreFinished(int, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Beginning restoreFinished with exitstatus:" << exitStatus;
    ui->backupConsole->insertPlainText("Restore Complete");
    QMessageBox::warning(this, "Restore complete", "Restore complete");
    StandardWidget::enableMenuBar();

    // Check to see if the update failed
    if (restore_process->exitStatus() != QProcess::NormalExit)
    {
        ui->backupConsole->insertPlainText("\n Update Failed (Crashed): \n The Restore script has crashed with an error. \n Contact KIPR tech support for assistance if the problem persists \n");
        ui->backupConsole->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
    }

    qDebug() << "Before restore cleanup process";
    // Cleanup process
    ui->backupConsole->setProcess(0);


    // Re-enable buttons
    ui->backupoption->setEnabled(true);
    ui->restore->setEnabled(true);
}

void BackupWidget::restore()
{

    // Mount USB drive
    if (!this->mountUsb("/dev/sda1", BackupWidget::mountDir) && !this->mountUsb("/dev/sdb1", BackupWidget::mountDir))
        QMessageBox::warning(this, "USB not found", "Failed to mount USB device\n Please insert USB device and try again");
    else
    {
        // Verify with user that they want to do the backup
        if (QMessageBox::question(this, "Restore?",
                                  QString("Are you sure you want to restore your programs?\n This will overwrite all existing programs."),
                                  QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
            return;

        // Disable buttons
        ui->backupoption->setEnabled(false);
        ui->restore->setEnabled(false);
        StandardWidget::disableMenuBar();

        ui->BackupRestoreLabel->setText("Restore in progress...");
        restore_process = new QProcess();

        QString command = QString("/home/kipr/wombat-os/Backup/restore.sh");
        restore_process->start("sudo", QStringList() << "sh" << "-c" << command);
        ui->backupConsole->setProcess(restore_process);

        connect(restore_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(restoreFinished(int, QProcess::ExitStatus)));
    }
}

/*void BackupWidget::cleardrive()
{
    QProcess clear_process;
    clear_process.startDetached("/bin/sh", QStringList()<< "/home/pi/got2/Backup/ClearDrive.sh");
    clear_process.waitForFinished(); // sets current thread to sleep and waits for Clear end
    ui->backupConsole->insertPlainText("Backup Complete");
    QMessageBox::warning(this, "complete", "Clear Completed");
}*/
void BackupWidget::handleStandardOutput()
{
    QString output = backup_process->readAllStandardOutput();
    qDebug() << "Backup output: " << output;
    // Optionally display the output in the UI, like a text box or console window
    ui->backupConsole->append(output); // Example if you want to display the output in a QTextEdit or similar widget
}

void BackupWidget::handleStandardError()
{
    QString error = backup_process->readAllStandardError();

    if (error.contains("Invalid argument"))
    {
        error.replace("Invalid argument", "Special character found, not backed up.");
    }
    else if (error.contains("cannot create directory"))
    {
        error.replace("cannot create directory", "Special character found, not backed up.");
    }

    // Regular expression to extract the file path between single quotes or any other pattern
    QRegularExpression filePathRegex("/mnt/[^ ]+([^\n]*)"); // Match paths starting with /mnt/

    QRegularExpressionMatchIterator i = filePathRegex.globalMatch(error); // Find all matches
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        QString filePath = match.captured(0); // Capture the full file path

        // Remove the '/mnt' prefix from the file path
        filePath.remove(0, 4);

        // Add the file path to the error list only
        if (!filePath.isEmpty())
        {
            errorFiles.append(filePath); // Only store the file path, no error message
        }
    }
    qDebug() << "Backup error: " << error;
    ui->backupConsole->append(error); // Display the error in a QTextEdit or similar widget
}