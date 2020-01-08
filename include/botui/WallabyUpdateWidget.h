#ifndef _WALLABYUPDATEWIDGET_H_
#define _WALLABYUPDATEWIDGET_H_

#include "StandardWidget.h"

#include <QString>
#include <QProcess>
#include <QDir>

namespace Ui
{
  class WallabyUpdateWidget;
}

class WallabyUpdateWidget : public StandardWidget
{
Q_OBJECT
public:
  WallabyUpdateWidget(Device *device, QWidget *parent = 0);
  ~WallabyUpdateWidget();
  
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
  Ui::WallabyUpdateWidget *ui;
  QProcess *m_updateProc;
};

#endif