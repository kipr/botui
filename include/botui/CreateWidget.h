#ifndef _CREATEWIDGET_H_
#define _CREATEWIDGET_H_

#include "StandardWidget.h"

class CreateWidget : public StandardWidget {
    Q_OBJECT
  public:
    CreateWidget(Device *device, QWidget *parent = 0);
    ~CreateWidget();

  private slots:
    void drive();

  private:
};

#endif
