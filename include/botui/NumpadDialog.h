#ifndef NumpadDialog_H
#define NumpadDialog_H

#include "InputProviderDialog.h"
#include "KeyButton.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <limits>

namespace Ui {
class NumpadDialog;
}

class NumpadDialog : public InputProviderDialog {
    Q_OBJECT
  public:
    enum InputType { Integer, Decimal };

    NumpadDialog(const QString &text,
                 InputType type = Integer,
                 const double &min = -std::numeric_limits<double>::max(),
                 const double &max = std::numeric_limits<double>::max(),
                 QWidget *parent = 0);
    ~NumpadDialog();

    void setInput(const QString &input);
    QString input() const;

  private slots:
    void displayChanged(QString);
    void numberPressed();
    void delPressed();
    void clearPressed();
    void decimalPressed();
    void signPressed();

  private:
    Ui::NumpadDialog *ui;
    QGridLayout *grid;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *display;

    bool m_decimalExists;
    bool m_isEmpty;
    double m_min;
    double m_max;

    KeyButton *makeButton(const char *, const QString &, const QString & = "");
    bool inBounds(const double &) const;
};

#endif // NumpadDialog_H
