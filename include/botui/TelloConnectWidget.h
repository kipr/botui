#ifndef _TELLOCONNECTWIDGET_H_
#define _TELLOCONNECTWIDGET_H_

#include "Options.h"

#ifdef NETWORK_ENABLED

#include "StandardWidget.h"
#include "NetworkManager.h"

#include <QProcess>
#include <QMessageBox>

class QTimer;

class NetworkSettingsWidget;

namespace Ui
{
    class TelloConnectWidget;
}

class TelloConnectWidget : public StandardWidget
{
    Q_OBJECT
public:
    // TelloConnectWidget(Device *device, QWidget *parent = 0);
    ~TelloConnectWidget();
    TelloConnectWidget(NetworkSettingsWidget *nsw, Device *device, QWidget *parent = 0);

public slots:
   
    void manage();
    void updateInformation();
    QString getCurrentConnectionMode();
    void enableClientMode();
    void connectToButton();

private slots:
    void stateChanged(const NetworkManager::State &newState, const NetworkManager::State &oldState);
    void indexChanged(int index);

private:
    NetworkSettingsWidget *nsw;
    Ui::TelloConnectWidget *ui;
    QTimer *enableCoolOffTimer;
    QProcess proc;
    QMessageBox msgBox;
};

#endif

#endif
