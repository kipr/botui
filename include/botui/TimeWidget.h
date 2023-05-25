#ifndef _TIMEWIDGET_H_
#define _TIMEWIDGET_H_

#include <QLabel>

class TimeWidget : public QLabel {
    Q_OBJECT
  public:
    TimeWidget(QWidget *parent = 0);
  private slots:
    void updateTime();
};

#endif
