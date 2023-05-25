#ifndef _PROGRAMSELECTIONWIDGET_H_
#define _PROGRAMSELECTIONWIDGET_H_
#include <QWidget>

namespace Ui {
class ProgramSelectionWidget;
}

class MenuBar;
class StatusBar;
class Device;

class ProgramSelectionWidget : public QWidget {
    Q_OBJECT
  public:
    ProgramSelectionWidget(Device *device, QWidget *parent = 0);
    ~ProgramSelectionWidget();

  public slots:
    void runningProgram();
    void otherPrograms();

  private:
    Ui::ProgramSelectionWidget *ui;
    Device *m_device;
    MenuBar *m_menuBar;
    StatusBar *m_statusBar;
};

#endif
