#ifndef _COMMUNICATIONSETTINGSWIDGET_H_
#define _COMMUNICATIONSETTINGSWIDGET_H_

#include "StandardWidget.h"

namespace Ui {
class CommunicationSettingsWidget;
}

class CommunicationSettingsWidget : public StandardWidget {
    Q_OBJECT
  public:
    CommunicationSettingsWidget(Device *device, QWidget *parent = 0);
    ~CommunicationSettingsWidget();

  private slots:
    void deviceNameChanged(const QString &text);
    void passwordedChanged(const int state);
    void passwordChanged(const QString &text);

  private:
    Ui::CommunicationSettingsWidget *ui;
};

#endif