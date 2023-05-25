#ifndef _PROGRAMARGSWIDGET_H_
#define _PROGRAMARGSWIDGET_H_

#include "StandardWidget.h"

namespace Ui {
class ProgramArgsWidget;
}

class QListWidgetItem;

class ProgramArgsWidget : public StandardWidget {
    Q_OBJECT
  public:
    ProgramArgsWidget(const QString &program,
                      Device *device,
                      QWidget *parent = 0);
    ~ProgramArgsWidget();

  private slots:
    void edit();
    void up();
    void down();

    void add();
    void remove();

    void currentItemChanged(QListWidgetItem *current);

  private:
    Ui::ProgramArgsWidget *ui;
    QString m_name;

    bool m_dirty;
};

#endif
