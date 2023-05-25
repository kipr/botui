#ifndef _COMPILINGWIDGET_H_
#define _COMPILINGWIDGET_H_

#include "StandardWidget.h"

namespace Ui {
class CompilingWidget;
}

class CompilingWidget : public StandardWidget {
    Q_OBJECT
  public:
    CompilingWidget(Device *device, QWidget *parent = 0);
    ~CompilingWidget();

  private slots:
    void compileFinished();

  private:
    Ui::CompilingWidget *ui;
};

#endif
