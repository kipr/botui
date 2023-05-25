#ifndef KEYBOARDDIALOG_H
#define KEYBOARDDIALOG_H

#include "InputProviderDialog.h"
#include "KeyButton.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

namespace Ui {
class KeyboardDialog;
}

class KeyboardDialog : public InputProviderDialog {
    Q_OBJECT
    Q_PROPERTY(QString input READ input)

  public:
    enum Mode { Normal, Password };

    KeyboardDialog(const QString &text,
                   Mode mode = Normal,
                   QWidget *parent = 0);
    ~KeyboardDialog();

    void setInput(const QString &input);
    QString input() const;

  private slots:
    void symbolPressed();
    void spacePressed();
    void shiftPressed();
    void delPressed();
    void numPressed();
    void symPressed();

  private:
    Ui::KeyboardDialog *ui;
    QLabel *label;
    QLineEdit *display;
    QVBoxLayout *alphaVerticalLayout;
    QVBoxLayout *numVerticalLayout;

    KeyButton *alphaButtons[26];
    KeyButton *numButtons[25];
    KeyButton *enterButton;
    KeyButton *spaceButton;
    KeyButton *shiftButton;
    KeyButton *delButton;
    KeyButton *numButton;
    KeyButton *symButton;

    bool shifted;

    KeyButton *makeButton(const char *, const QString &, const QString & = "");
    void setupAlphaButtons();
    void setupNumButtons();
    void setupSpecialButtons();
};

#endif // KEYBOARDDIALOG_H
