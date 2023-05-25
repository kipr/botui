#ifndef _LANGUAGEWIDGET_H_
#define _LANGUAGEWIDGET_H_

#include "StandardWidget.h"

namespace Ui {
class LanguageWidget;
}

class LanguageWidget : public StandardWidget {
    Q_OBJECT
  public:
    LanguageWidget(Device *device, QWidget *const parent = 0);
    ~LanguageWidget();

  private Q_SLOTS:
    void currentIndexChanged(int index);

  private:
    Ui::LanguageWidget *ui;
};

#endif