#ifndef _NOTYETIMPLEMENTEDDIALOG_H_
#define _NOTYETIMPLEMENTEDDIALOG_H_

#include <QDialog>

namespace Ui {
class NotYetImplementedDialog;
}

class NotYetImplementedDialog : public QDialog {
  public:
    NotYetImplementedDialog(QWidget *parent = 0);
    ~NotYetImplementedDialog();

    static void nyi();

  private:
    Ui::NotYetImplementedDialog *ui;
};

#endif
