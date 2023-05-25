#ifndef _PROGRAMWIDGET_H_
#define _PROGRAMWIDGET_H_

#include "ButtonProvider.h"
#include "StandardWidget.h"
#include <QAction>
#include <QProcess>
#include <QPushButton>

namespace Ui {
class ProgramWidget;
}

class Program;

class ProgramWidget : public StandardWidget {
    Q_OBJECT
  public:
    ProgramWidget(Program *program, Device *device, QWidget *parent = 0);
    ~ProgramWidget();

  public slots:
    void lock();
    void started();
    void stoppressed();

  private slots:
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

    void buttonTextChanged(const ButtonProvider::ButtonId &id,
                           const QString &text);
    void extraShownChanged(const bool &shown);

    void aPressed();
    void bPressed();
    void cPressed();
    void xPressed();
    void yPressed();
    void zPressed();

    void aReleased();
    void bReleased();
    void cReleased();
    void xReleased();
    void yReleased();
    void zReleased();

  private:
    Ui::ProgramWidget *ui;
    Program *m_program;
    bool m_running;
    // QPushButton *m_button;
};

#endif
