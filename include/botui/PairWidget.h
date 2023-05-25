#ifndef _PAIRWIDGET_H_
#define _PAIRWIDGET_H_

#include <QWidget>

namespace Ui {
class PairWidget;
}

class MenuBar;
class StatusBar;
class Device;

class PairWidget : public QWidget {
    Q_OBJECT
  public:
    PairWidget(Device *device, QWidget *parent = 0);
    ~PairWidget();

    void setPassword(const QString &password);

  signals:
    void cancel();

  private:
    Ui::PairWidget *ui;
    Device *m_device;
    MenuBar *m_menuBar;
};

#endif
