#ifndef _GUISETTINGSWIDGET_H_
#define _GUISETTINGSWIDGET_H_

#include "StandardWidget.h"

namespace Ui {
class GuiSettingsWidget;
}

class GuiSettingsWidget : public StandardWidget {
    Q_OBJECT
  public:
    GuiSettingsWidget(Device *device, QWidget *parent = 0);
    ~GuiSettingsWidget();

    static void updateStyle(Device *device);

  private slots:
    void fullscreenChanged(int state);
    void hideUIChanged(int state);
    void on_invert_screen_clicked();

  private:
    static void updateWidgets();
    Ui::GuiSettingsWidget *ui;
};

#endif
