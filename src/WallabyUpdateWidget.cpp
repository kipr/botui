#include "WallabyUpdateWidget.h"
#include "ui_WallabyUpdateWidget.h"

#include <QDir>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>

WallabyUpdateWidget::WallabyUpdateWidget(Device *device, QWidget *parent)
  : StandardWidget(device, parent),
  ui(new Ui::WallabyUpdateWidget)
{
  ui->setupUi(this);
  performStandardSetup("Update");
  
  ui->updateConsole->setVisible(false);
  
  connect(ui->update, SIGNAL(clicked()), SLOT(update()));
  connect(ui->refresh, SIGNAL(clicked()), SLOT(refresh()));
  connect(ui->ethernet, SIGNAL(clicked()), SLOT(ethernet()));	
}

WallabyUpdateWidget::~WallabyUpdateWidget()
{
  delete ui;
}
  
void WallabyUpdateWidget::update()
{ 
  // Get single selected item
  const QList<QListWidgetItem *> selected = ui->updateList->selectedItems();
  if(selected.size() != 1)
    return;
  const QString selectedName = selected.at(0)->text();
  
  // Verify with user that they want to do the update
  if(QMessageBox::question(this, "Update?",
    QString("Are you sure you want to update using %1?").arg(selectedName),
    QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
      return;
  
  // Disable buttons
  ui->update->setEnabled(false);
  ui->refresh->setEnabled(false);
  ui->ethernet->setEnabled(false);

  // Mount USB drive
  if(!this->mountUsb("/dev/sda1", WallabyUpdateWidget::mountDir) && !this->mountUsb("/dev/sdb1", WallabyUpdateWidget::mountDir))
    QMessageBox::warning(this, "USB not found", "Failed to mount USB device");
  else {
    // Check if update file exists
    QDir subDir = WallabyUpdateWidget::mountDir;
    subDir.cd(selectedName);
    if(!subDir.exists(WallabyUpdateWidget::updateFileName))
      QMessageBox::warning(this, "File not found", "The update file no longer exists");
    else {
      // Change UI to show output
      ui->updateConsole->setVisible(true);
      ui->selectionWidget->setVisible(false);
      ui->statusLabel->setText("Update progress:");
      
      // Run update process
      m_updateProc = new QProcess();
      m_updateProc->setProcessChannelMode(QProcess::MergedChannels);
      m_updateProc->setWorkingDirectory(subDir.absolutePath());
      ui->updateConsole->setProcess(m_updateProc);
      connect(m_updateProc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(updateFinished(int, QProcess::ExitStatus)));
      m_updateProc->start("sh", QStringList() << WallabyUpdateWidget::updateFileName);
      
      // Update script will reboot the controller
    }
  }
}

void WallabyUpdateWidget::refresh()
{
  ui->refresh->setEnabled(false);
  
  // Clear items
  ui->updateList->clear();
  
  // Mount USB drive
  if(!this->mountUsb("/dev/sda1", WallabyUpdateWidget::mountDir) && !this->mountUsb("/dev/sdb1", WallabyUpdateWidget::mountDir))
    QMessageBox::warning(this, "USB not found", "Failed to mount USB device");
  else {
    // Look at each directory
    foreach(const QString &name, WallabyUpdateWidget::mountDir.entryList(QDir::NoDotAndDotDot | QDir::Dirs))
    {
      // Filter out directories without updates
      QDir subDir = WallabyUpdateWidget::mountDir;
      subDir.cd(name);
      
      if(!subDir.exists(WallabyUpdateWidget::updateFileName))
        continue;
    
      // Add directory to the list
      ui->updateList->addItem(subDir.dirName());
    }
    
    // Unmount USB drive
    if(!this->unmountUsb(WallabyUpdateWidget::mountDir.absolutePath()))
      qDebug() << "Failed to unmount USB drive";
  }
  
  ui->refresh->setEnabled(true);
}

void WallabyUpdateWidget::updateFinished(int, QProcess::ExitStatus exitStatus)
{
  // Cleanup process
  ui->updateConsole->setProcess(0);
  delete m_updateProc;
  
  // Unmount USB drive
  if(!this->unmountUsb(WallabyUpdateWidget::mountDir.absolutePath()))
    qDebug() << "Failed to unmount USB drive";
  
  // Re-enable buttons
  ui->refresh->setEnabled(true);
  ui->update->setEnabled(true);
}

bool WallabyUpdateWidget::mountUsb(const QString device, const QDir dir)
{
  QProcess proc;
  proc.start("mount", QStringList() << device << dir.absolutePath());
  return proc.waitForFinished(5000) && proc.exitCode() == 0;
}

bool WallabyUpdateWidget::unmountUsb(const QString device)
{
  QProcess proc;
  proc.start("umount", QStringList() << device);
  return proc.waitForFinished(5000) && proc.exitCode() == 0;
}

const QString WallabyUpdateWidget::updateFileName = "wombat_update.sh";
const QDir WallabyUpdateWidget::mountDir = QDir("/mnt");

void WallabyUpdateWidget::ethernet(){
        if(QMessageBox::question(this, "Update?", QString("Is the ethernet cable plugged into the controller?"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
      return;



	// Change UI to show output
      ui->updateConsole->setVisible(true);
      ui->selectionWidget->setVisible(false);
      ui->statusLabel->setText("Update progress:");
      
      // Run update process
      m_updateProc = new QProcess();
      m_updateProc->setProcessChannelMode(QProcess::MergedChannels);
      ui->updateConsole->setProcess(m_updateProc);
      connect(m_updateProc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(updateFinished(int, QProcess::ExitStatus)));
      m_updateProc->start("sh /home/pi/updateMe.sh");


}


