#ifndef _UNDERCONSTRUCTIONWIDGET_H_
#define _UNDERCONSTRUCTIONWIDGET_H_

#include <QWidget>

namespace Ui {
class UnderConstructionWidget;
}

class MenuBar;
class StatusBar;
class Device;

class UnderConstructionWidget : public QWidget {
    Q_OBJECT
  public:
    UnderConstructionWidget(Device *device, QWidget *parent = 0);
    ~UnderConstructionWidget();

  public slots:
    void back();
    void home();

  private:
    Ui::UnderConstructionWidget *ui;
    Device *m_device;
    MenuBar *m_menuBar;
    StatusBar *m_statusBar;
};

#endif
