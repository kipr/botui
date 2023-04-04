#include "TouchSlider.h"
#include <QDebug>

TouchSlider::TouchSlider(QWidget *parent) : QSlider(parent), _label(0xFFFF){};
quint16 TouchSlider::label() { return _label; }
void TouchSlider::setLabel(quint16 label) { _label = label; }