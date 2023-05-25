#include "Options.h"

#ifdef NETWORK_ENABLED

#ifndef _OTHERNETWORKWIDGET_H_
#define _OTHERNETWORKWIDGET_H_

#include "Network.h"
#include "StandardWidget.h"

namespace Ui {
class OtherNetworkWidget;
}

class KeyboardDialog;

class OtherNetworkWidget : public StandardWidget {
    Q_OBJECT
  public:
    OtherNetworkWidget(Device *device, QWidget *parent = 0);
    ~OtherNetworkWidget();

    void fillNetworkInfo(const Network &network);

  private slots:
    void join();
    void securityChanged(int index);

  private:
    Ui::OtherNetworkWidget *ui;

    KeyboardDialog *m_ssid;
    KeyboardDialog *m_password;
};

#endif

#endif