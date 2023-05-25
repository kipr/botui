#ifndef _ACCESSORIESWIDGET_H_
#define _ACCESSORIESWIDGET_H_

#include "StandardWidget.h"

namespace Ui {
class AccessoriesWidget;
}

class AccessoriesWidget : public StandardWidget {
    Q_OBJECT
  public:
    AccessoriesWidget(Device *device, QWidget *parent = 0);
    ~AccessoriesWidget();

  private slots:
    void create();
    void ardrone();

  private:
    Ui::AccessoriesWidget *ui;
};

#endif
