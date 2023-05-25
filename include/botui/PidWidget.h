#ifndef _PIDWIDGET_H_
#define _PIDWIDGET_H_

#include "StandardWidget.h"

namespace Ui {
class PidWidget;
}

class QPushButton;

class PidWidget : public StandardWidget {
    Q_OBJECT
  public:
    PidWidget(Device *device, QWidget *parent = 0);
    ~PidWidget();

  private slots:
    void portClicked();
    void go();

  private:
    Ui::PidWidget *ui;
    QPushButton *m_ports[4];
    quint8 m_currentPort;
};

#endif
