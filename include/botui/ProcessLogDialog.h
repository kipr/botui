#ifndef _PROCESS_LOG_DIALOG_H_
#define _PROCESS_LOG_DIALOG_H_

#include "LogDialog.h"

class QProcess;

class ProcessLogDialog : public LogDialog {
    Q_OBJECT
  public:
    ProcessLogDialog(QWidget *const parent = 0);

    bool start(const QString &path, const QStringList &args = QStringList());

  private slots:
    void readOutput();
    void terminated();

  private:
    QProcess *_process;
};

#endif