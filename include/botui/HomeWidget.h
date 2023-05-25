#ifndef _HOMEWIDGET_H_
#define _HOMEWIDGET_H_

#include "StandardWidget.h"

namespace Ui {
class HomeWidget;
}

class HomeWidget : public StandardWidget {
    Q_OBJECT
  public:
    HomeWidget(Device *device, QWidget *parent = 0);
    ~HomeWidget();

  public slots:
    void programs();
    void fileManager();
    void motorsSensors();
    void settings();
    void about();
    void shutDown();
    void lock();

  private:
    Ui::HomeWidget *ui;
};

#endif
