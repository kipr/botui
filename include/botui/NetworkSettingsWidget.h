#ifndef _NETWORKSETTINGSWIDGET_H_
#define _NETWORKSETTINGSWIDGET_H_

#include "Options.h"

#ifdef NETWORK_ENABLED

#include "NetworkManager.h"
#include "StandardWidget.h"

#include <QMessageBox>
#include <QProcess>

class QTimer;

namespace Ui {
class NetworkSettingsWidget;
}

class NetworkSettingsWidget : public StandardWidget {
    Q_OBJECT
  public:
    NetworkSettingsWidget(Device *device, QWidget *parent = 0);
    ~NetworkSettingsWidget();

  public slots:
    void TournamentMode();
    void connect();
    void manage();
    void updateInformation();

  private slots:
    void stateChanged(const NetworkManager::State &newState,
                      const NetworkManager::State &oldState);
    void indexChanged(int index);

  private:
    Ui::NetworkSettingsWidget *ui;
    QTimer *enableCoolOffTimer;
    QProcess proc;
    QMessageBox msgBox;
    bool firstTimeSetup;
    void setupConnectionModeSelect();
};

#endif

#endif
