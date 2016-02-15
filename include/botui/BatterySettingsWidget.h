#ifndef _BATTERYSETTINGSWIDGET_H_
#define _BATTERYSETTINGSWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
  class BatterySettingsWidget;
}

class BatterySettingsWidget : public StandardWidget
{
Q_OBJECT
public:
  BatterySettingsWidget(Device *device, QWidget *const parent = 0);
  ~BatterySettingsWidget();
  
private slots:
  void typeChanged(int type);
  
private:
  Ui::BatterySettingsWidget *ui;
};

#endif