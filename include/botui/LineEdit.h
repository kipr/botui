#ifndef _LINEEDIT_H_
#define _LINEEDIT_H_

#include <InputProviderDialog.h>
#include <QLineEdit>

class InputProviderDialog;

class LineEdit : public QLineEdit {
    Q_OBJECT
    Q_PROPERTY(InputProviderDialog *inputProvider READ inputProvider WRITE
                   setInputProvider)
  public:
    LineEdit(QWidget *parent = 0);
    LineEdit(InputProviderDialog *inputProvider, QWidget *parent = 0);
    virtual bool event(QEvent *e);

    void setInputProvider(InputProviderDialog *inputProvider);
    InputProviderDialog *inputProvider() const;

  protected:
    void paintEvent(QPaintEvent *e);

  private:
    void init();

    InputProviderDialog *m_inputProvider;
};

#endif
