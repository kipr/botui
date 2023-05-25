#ifndef _BATTERYSETTINGSWIDGET_H_
#define _BATTERYSETTINGSWIDGET_H_

#include "NumpadDialog.h"
#include "StandardWidget.h"

namespace Ui {
class BatterySettingsWidget;
}

class BatterySettingsWidget : public StandardWidget {
    Q_OBJECT
  public:
    BatterySettingsWidget(Device *device, QWidget *const parent = 0);
    ~BatterySettingsWidget();

  private slots:
    void typeChanged(int type);
    void warnEnabledChanged(int state);
    void warnThreshChanged(QString text);

  private:
    Ui::BatterySettingsWidget *ui;
    NumpadDialog *m_numpadInput;

    int m_currType;
};

#endif