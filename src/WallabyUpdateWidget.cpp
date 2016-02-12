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
  
  // TODO: Move this out of constructor
  this->refresh();
}

WallabyUpdateWidget::~WallabyUpdateWidget()
{
  delete ui;
}
  
void WallabyUpdateWidget::update()
{
  // TODO: Verify with user that they want to do the update
  
  // Get single selected item
  const QList<QListWidgetItem *> selected = ui->updateList->selectedItems();
  if(selected.size() != 1)
    return;
  const QString selectedName = selected.at(0)->text();
  
  // Disable buttons
  ui->update->setEnabled(false);
  ui->refresh->setEnabled(false);
  
  // Mount USB drive
  const QDir mountDir("/mnt");
  if(!this->mountUsb("/dev/sda1", mountDir) && !this->mountUsb("/dev/sdb1", mountDir))
    QMessageBox::warning(this, "USB not found", "Failed to mount USB device");
  else {
    // Check if update file exists
    QDir subDir = mountDir;
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
      ui->updateConsole->setProcess(m_updateProc);
      m_updateProc->start("sh", QStringList() << subDir.absoluteFilePath(WallabyUpdateWidget::updateFileName));
      m_updateProc->waitForFinished(-1);
      
      // Update script will reboot the controller
      
      // Cleanup process
      ui->updateConsole->setProcess(0);
      delete m_updateProc;
    }
    
    // Unmount USB drive
    if(!this->unmountUsb(mountDir.absolutePath()))
      qDebug() << "Failed to unmount USB drive";
  }
  
  // Re-enable buttons
  ui->refresh->setEnabled(true);
  ui->update->setEnabled(true);
}

void WallabyUpdateWidget::refresh()
{
  ui->refresh->setEnabled(false);
  
  // Clear items
  ui->updateList->clear();
  
  // Mount USB drive
  const QDir mountDir("/mnt");
  if(!this->mountUsb("/dev/sda1", mountDir) && !this->mountUsb("/dev/sdb1", mountDir))
    QMessageBox::warning(this, "USB not found", "Failed to mount USB device");
  else {
    // Look at each directory
    foreach(const QString &name, mountDir.entryList(QDir::NoDotAndDotDot | QDir::Dirs))
    {
      // Filter out directories without updates
      QDir subDir = mountDir;
      subDir.cd(name);
      
      if(!subDir.exists(WallabyUpdateWidget::updateFileName))
        continue;
    
      // Add directory to the list
      ui->updateList->addItem(subDir.dirName());
    }
    
    // Unmount USB drive
    if(!this->unmountUsb(mountDir.absolutePath()))
      qDebug() << "Failed to unmount USB drive";
  }
  
  ui->refresh->setEnabled(true);
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

const QString WallabyUpdateWidget::updateFileName = "wallaby_update.sh";