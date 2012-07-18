#ifndef QKEYBUTTON_H
#define QKEYBUTTON_H

#include <QAbstractButton>

class QKeyButton : public QAbstractButton
{
    Q_OBJECT
public:
    QKeyButton(QString firstlabel, QString secondLabel, QWidget *parent = 0);

    bool isSwitched();

public slots:
    void switchLabel();

protected:
    void paintEvent(QPaintEvent *);

private:
    QString m_firstLabel;
    QString m_secondLabel;
    bool m_switched;
};

#endif // QKEYBUTTON_H
