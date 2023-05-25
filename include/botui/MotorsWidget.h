#ifndef _MOTORSWIDGET_H_
#define _MOTORSWIDGET_H_

#include "StandardWidget.h"

namespace Ui {
class MotorsWidget;
}

class InputProviderDialog;

class MotorsWidget : public StandardWidget {
    Q_OBJECT
  public:
    MotorsWidget(Device *device, QWidget *parent = 0);
    ~MotorsWidget();

  private slots:
    void valueChanged(const double &value);
    void activeChanged();

    void manualEntry(const QString &text);

    void update();

  private:
    Ui::MotorsWidget *ui;
    InputProviderDialog *m_provider;
};

#endif
