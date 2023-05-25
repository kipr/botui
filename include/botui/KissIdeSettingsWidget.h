#ifndef _KISSIDESETTINGSWIDGET_H_
#define _KISSIDESETTINGSWIDGET_H_

#include "StandardWidget.h"

namespace Ui {
class KissIdeSettingsWidget;
}

class KissIdeSettingsWidget : public StandardWidget {
    Q_OBJECT
  public:
    KissIdeSettingsWidget(Device *device, QWidget *parent = 0);
    ~KissIdeSettingsWidget();

  private:
    Ui::KissIdeSettingsWidget *ui;
};

#endif
