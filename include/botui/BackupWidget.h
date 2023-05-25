#ifndef _BACKUPWIDGET_H_
#define _BACKUPWIDGET_H_

#include "Device.h"
#include "StandardWidget.h"

namespace Ui {
class BackupWidget;
}

class BackupWidget : public StandardWidget {
    Q_OBJECT
  public:
    BackupWidget(Device *device, QWidget *widget = 0);
    ~BackupWidget();

  public slots:
    void backupoption();
    // void cleardrive();
    void restore();

  private:
    Ui::BackupWidget *ui;
};

#endif
