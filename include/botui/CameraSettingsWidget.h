#ifndef _CAMERASETTINGSWIDGET_H_
#define _CAMERASETTINGSWIDGET_H_

#include "StandardWidget.h"

namespace Ui {
class CameraSettingsWidget;
}

class CameraSettingsWidget : public StandardWidget {
    Q_OBJECT
  public:
    CameraSettingsWidget(Device *device, QWidget *const parent = 0);
    ~CameraSettingsWidget();

  public slots:
    void minus();
    void plus();

  private:
    Ui::CameraSettingsWidget *ui;
};

#endif
