#ifndef _CREATECHANNELDIALOG_H_
#define _CREATECHANNELDIALOG_H_

#include <QDialog>

namespace Ui {
class CreateChannelDialog;
}

class CreateChannelDialog : public QDialog {
  public:
    CreateChannelDialog(QWidget *parent = 0);
    ~CreateChannelDialog();

    QString type() const;

  private:
    Ui::CreateChannelDialog *ui;
};

#endif
