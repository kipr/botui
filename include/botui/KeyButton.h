#ifndef KEYBUTTON_H
#define KEYBUTTON_H

#include <QAbstractButton>

class KeyButton : public QAbstractButton {
    Q_OBJECT
  public:
    KeyButton(const QString &firstlabel,
              const QString &secondLabel,
              QWidget *parent = 0);

    bool isSwitched() const;

  public slots:
    void switchLabel();

  protected:
    void paintEvent(QPaintEvent *);

  private:
    QString m_firstLabel;
    QString m_secondLabel;
    bool m_switched;
};

#endif // KEYBUTTON_H
