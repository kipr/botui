#ifndef _ADVANCEDSETTINGSWIDGET_H_
#define _ADVANCEDSETTINGSWIDGET_H_

#include "StandardWidget.h"

namespace Ui {
class AdvancedSettingsWidget;
}

class AdvancedSettingsWidget : public StandardWidget {
    Q_OBJECT
  public:
    AdvancedSettingsWidget(Device *device, QWidget *parent = 0);
    ~AdvancedSettingsWidget();

  public slots:
    void network();
    void gui();
    void battery();
    void factory();

  private:
    Ui::AdvancedSettingsWidget *ui;
};

#endif
