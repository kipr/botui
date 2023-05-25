#include "Options.h"

#ifdef NETWORK_ENABLED

#ifndef _MANAGENETWORKS_H_
#define _MANAGENETWORKS_H_

#include "StandardWidget.h"

namespace Ui {
class ManageNetworks;
}

class NetworkItemModel;

class ManageNetworksWidget : public StandardWidget {
    Q_OBJECT
  public:
    ManageNetworksWidget(Device *device, QWidget *parent = 0);
    ~ManageNetworksWidget();

  private slots:
    void forget();

  private:
    Ui::ManageNetworks *ui;
    Device *m_device;
    NetworkItemModel *m_model;
};

#endif

#endif