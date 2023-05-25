#include "Options.h"

#ifdef NETWORK_ENABLED

#ifndef _NETWORKSTATUSWIDGET_H_
#define _NETWORKSTATUSWIDGET_H_

#include <QWidget>

class NetworkingProvider;

class NetworkStatusWidget : public QWidget {
    Q_OBJECT
  public:
    NetworkStatusWidget(QWidget *parent = 0);

    static bool isNetworkUp(const std::string networkName = "wlan0");

  protected:
    void paintEvent(QPaintEvent *event);

  private slots:
    void networkStateChanged();

  private:
    void constrain();
};

#endif

#endif
