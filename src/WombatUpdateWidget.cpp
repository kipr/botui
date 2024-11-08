#include "WombatUpdateWidget.h"
#include "ui_WombatUpdateWidget.h"

#include <QDir>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>

WombatUpdateWidget::WombatUpdateWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::WombatUpdateWidget)
{
  ui->setupUi(this);
  performStandardSetup("Update");

  ui->updateConsole->setVisible(false);

  connect(ui->update, SIGNAL(clicked()), SLOT(update()));
  connect(ui->refresh, SIGNAL(clicked()), SLOT(refresh()));
  connect(ui->ethernet, SIGNAL(clicked()), SLOT(ethernet()));
}

WombatUpdateWidget::~WombatUpdateWidget()
{
  delete ui;
}

void WombatUpdateWidget::update()
{
  // Get single selected item
  const QList<QListWidgetItem *> selected = ui->updateList->selectedItems();
  if (selected.size() != 1)
    return;
  const QString selectedName = selected.at(0)->text();

  // Verify with user that they want to do the update
  if (QMessageBox::question(this, "Update?",
                            QString("Are you sure you want to update using %1?").arg(selectedName),
                            QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
    return;

  // Disable buttons
  ui->update->setEnabled(false);
  ui->refresh->setEnabled(false);
  ui->ethernet->setEnabled(false);

  // Mount USB drive
  if (!this->mountUsb("/dev/sda1", WombatUpdateWidget::mountDir) && !this->mountUsb("/dev/sdb1", WombatUpdateWidget::mountDir))
    QMessageBox::warning(this, "USB not found", "Failed to mount USB device");
  else
  {

    // Check if selectedName contains ".zip"
    if (!selectedName.contains(".zip", Qt::CaseInsensitive))
    {
      QMessageBox::warning(this, "File Not Found", "The selected folder is not a .zip folder");
    }
    else
    {
      // Change UI to show output
      ui->updateConsole->setVisible(true);
      ui->selectionWidget->setVisible(false);
      ui->statusLabel->setText("Update progress:");

      // Run update process
      m_updateProc = new QProcess();
      m_updateProc->setProcessChannelMode(QProcess::MergedChannels);
      m_updateProc->setWorkingDirectory("/home/kipr");
      ui->updateConsole->setProcess(m_updateProc);
      connect(m_updateProc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(updateFinished(int, QProcess::ExitStatus)));
      m_updateProc->start("sh", QStringList() << "-c" << "/home/kipr/updateMe.sh /media/kipr/*/*");

      // Update script will reboot the controller
    }
  }
}

void WombatUpdateWidget::refresh()
{
  ui->refresh->setEnabled(false);

  // Clear items
  ui->updateList->clear();

  // Mount USB drive
  if (!this->mountUsb("/dev/sda1", WombatUpdateWidget::mountDir) && !this->mountUsb("/dev/sdb1", WombatUpdateWidget::mountDir))
    QMessageBox::warning(this, "USB not found", "Failed to mount USB device");
  else
  {
    // Look at each directory
    foreach (const QString &file, WombatUpdateWidget::mountDir.entryList(QDir::Files))
    {
      // Filter out files that aren't zip files
      if (!file.contains(".zip", Qt::CaseInsensitive))
        continue;

      // Add directory to the list
      ui->updateList->addItem(file);
    }

    // Unmount USB drive
    if (!this->unmountUsb(WombatUpdateWidget::mountDir.absolutePath()))
      qDebug() << "Failed to unmount USB drive";
  }

  ui->refresh->setEnabled(true);
}

void WombatUpdateWidget::updateFinished(int, QProcess::ExitStatus exitStatus)
{

  StandardWidget::enableMenuBar();

  // Check to see if the update failed
  if (m_updateProc->exitStatus() != QProcess::NormalExit)
  {
    ui->updateConsole->insertPlainText("\n Update Failed (Crashed): \n The update script has crashed with an error. \n Contact KIPR tech support for assistance if the problem persists \n");
    ui->updateConsole->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
  }

  // Cleanup process
  ui->updateConsole->setProcess(0);
  delete m_updateProc;

  // Unmount USB drive
  if (!this->unmountUsb(WombatUpdateWidget::mountDir.absolutePath()))
    qDebug() << "Failed to unmount USB drive";

  // Re-enable buttons
  ui->refresh->setEnabled(true);
  ui->update->setEnabled(true);
}

bool WombatUpdateWidget::mountUsb(const QString device, const QDir dir)
{
  QProcess proc;
  proc.start("mount", QStringList() << device << dir.absolutePath());
  return proc.waitForFinished(5000) && proc.exitCode() == 0;
}

bool WombatUpdateWidget::unmountUsb(const QString device)
{
  QProcess proc;
  proc.start("umount", QStringList() << device);
  return proc.waitForFinished(5000) && proc.exitCode() == 0;
}

const QString WombatUpdateWidget::updateFileName = "wombat_update.sh";
const QDir WombatUpdateWidget::mountDir = QDir("/mnt");

void WombatUpdateWidget::ethernet()
{
  if (QMessageBox::question(this, "Update?", QString("Is your controller connected to the internet over ethernet or Wifi?"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
  {
    return;
  }

  // Change UI to show output
  ui->updateConsole->setVisible(true);
  ui->selectionWidget->setVisible(false);
  ui->statusLabel->setText("Update progress:");

  // Disables menubar (Back and Home buttons) in StandardWidget
  StandardWidget::disableMenuBar();

  // Run update process
  m_updateProc = new QProcess();
  m_updateProc->setProcessChannelMode(QProcess::MergedChannels);
  ui->updateConsole->setProcess(m_updateProc);
  connect(m_updateProc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(updateFinished(int, QProcess::ExitStatus)));
  m_updateProc->startCommand("sudo sh /home/kipr/updateMe.sh");
}
