#ifndef _WALLABYUPDATEWIDGET_H_
#define _WALLABYUPDATEWIDGET_H_

#include "StandardWidget.h"

#include <QDir>
#include <QProcess>
#include <QString>

namespace Ui {
class WombatUpdateWidget;
}

class WombatUpdateWidget : public StandardWidget {
    Q_OBJECT
  public:
    WombatUpdateWidget(Device *device, QWidget *parent = 0);
    ~WombatUpdateWidget();

  public slots:
    void update();
    void refresh();
    void ethernet();

  private slots:
    void updateFinished(int exitCode, QProcess::ExitStatus exitStatus);

  private:
    bool mountUsb(const QString device, const QDir dir);
    bool unmountUsb(const QString device);

    static const QString updateFileName;
    static const QDir mountDir;
    Ui::WombatUpdateWidget *ui;
    QProcess *m_updateProc;
};

#endif