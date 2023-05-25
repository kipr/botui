#ifndef _INPUTPROVIDERDIALOG_H_
#define _INPUTPROVIDERDIALOG_H_

#include <QDialog>
#include <QString>

class InputProviderDialog : public QDialog {
    Q_OBJECT
    Q_PROPERTY(QString input READ input)
  public:
    InputProviderDialog(QWidget *parent = 0);

    virtual void setInput(const QString &input) = 0;
    virtual QString input() const = 0;
};

#endif
