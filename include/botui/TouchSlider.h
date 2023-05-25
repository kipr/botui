#ifndef __TOUCHSLIDER_H__
#define __TOUCHSLIDER_H__
#include <QSlider>

class TouchSlider : public QSlider {
  public:
    TouchSlider(QWidget *parent = nullptr);

    quint16 label();
    void setLabel(quint16 label);

  private:
    quint16 _label;
};

#endif