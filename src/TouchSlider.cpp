#include "TouchSlider.h"

TouchSlider::TouchSlider(QWidget *parent)
    : QSlider(Qt::Horizontal, parent), _label(0xFFFF) {
    QString style_sheet =
        tr("QSlider { min-width: 100px; height: 75px; } "
           ".QSlider::groove:horizontal {border: 1px solid #111; "
           "border-radius: 10px; background-color: black; margin: 2px 0; } "
           ".QSlider::handle:horizontal { background-color: red; border: 1px "
           "solid red; width: 30px; margin: -2px 0; border-radius: 10px; } ");
    this->setStyleSheet(style_sheet);
};
quint16 TouchSlider::label() { return _label; }
void TouchSlider::setLabel(quint16 label) { _label = label; }