#ifndef _PROGRAMSWIDGET_H_
#define _PROGRAMSWIDGET_H_

#include "StandardWidget.h"

#include <pcompiler/output.hpp>

namespace Ui {
class ProgramsWidget;
}

class ArchivesModel;
class ConcurrentCompile;

class ProgramsWidget : public StandardWidget {
    Q_OBJECT
  public:
    ProgramsWidget(Device *device, QWidget *parent = 0);
    ~ProgramsWidget();

  public slots:
    void run();
    void edit();
    void add();
    void args();
    void compile();
    void remove();
    void transfer();

  private slots:
    void compileStarted(const QString &name, ConcurrentCompile *compiler);
    void compileFinished(const Compiler::OutputList &output,
                         ConcurrentCompile *compiler);
    void update();

  private:
    Ui::ProgramsWidget *ui;
    ArchivesModel *m_model;
};

#endif
