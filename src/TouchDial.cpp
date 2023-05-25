#include "TouchDial.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <cmath>

// Qt specifies degrees in 1/16th angle increments
#define DEG_TO_ARCDEG(x) ((x)*16)

TouchDial::TouchDial(QWidget *parent)
    : QWidget(parent),
      m_minimumValue(-1.0),
      m_maximumValue(1.0),
      m_value(0.0),
      m_label(0xFFFF),
      m_readOnly(false) {}

TouchDial::~TouchDial() {}

void TouchDial::setMinimumValue(const double &minimumValue) {
    m_minimumValue = minimumValue;
    update();
}

const double &TouchDial::minimumValue() const { return m_minimumValue; }

void TouchDial::setMaximumValue(const double &maximumValue) {
    m_maximumValue = maximumValue;
    update();
}

const double &TouchDial::maximumValue() const { return m_maximumValue; }

void TouchDial::setValue(const double &value) {
    m_value = value;
    if (m_value < m_minimumValue) m_value = m_minimumValue;
    if (m_value > m_maximumValue) m_value = m_maximumValue;
    emit valueChanged(m_value);
    update();
}

const double &TouchDial::value() const { return m_value; }

void TouchDial::setLabel(const quint16 &label) {
    m_label = label;
    update();
}

const quint16 &TouchDial::label() const { return m_label; }

void TouchDial::setReadOnly(const bool &readOnly) { m_readOnly = readOnly; }

const bool &TouchDial::readOnly() const { return m_readOnly; }

void TouchDial::paintEvent(QPaintEvent *) {
    const int w = width();
    const int h = height();

    const int d = qMin(w, h) / 1.1;
    const int r = d / 2;

    const int xoff = (w - d) / 2;
    const int yoff = (h - d) / 2;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setPen(QPen(Qt::white, r / 25));

    p.setBrush(Qt::black);
    p.drawPie(xoff, yoff, d, d, DEG_TO_ARCDEG(-45), DEG_TO_ARCDEG(270));

    p.setBrush(Qt::red);

    const double angle = valueToAngle((m_value - m_minimumValue) /
                                      (m_maximumValue - m_minimumValue));
    p.setPen(QPen(Qt::red, r / 15));
    p.drawLine(xoff + r,
               yoff + r,
               xoff + r * (1 + cos(angle)),
               yoff + r * (1 + sin(angle)));
    p.setPen(QPen(Qt::white, r / 25));
    p.drawEllipse(QPoint(xoff + r, yoff + r), r / 4, r / 4);

    p.setPen(Qt::black);
    p.setBrush(Qt::black);
    QFont font = p.font();
    font.setPixelSize(r / 4);
    p.setFont(font);

    // This is to get rid of "-0" nonsense
    double printValue = m_value;
    if (printValue < 0.0 && printValue >= -0.5) printValue = 0.0;

    p.drawText(xoff + r * 0.6 + 0.5,
               yoff + r * 1.4 + 0.5,
               r * 0.8,
               r * 0.4,
               Qt::AlignHCenter | Qt::AlignVCenter,
               QString().asprintf("%.0f", printValue));

    if (m_label != 0xFFFF) {
        p.setPen(Qt::white);
        p.drawText(xoff + r * 0.6 + 0.5,
                   yoff + r * 0.6 + 0.5,
                   r * 0.8,
                   r * 0.8,
                   Qt::AlignHCenter | Qt::AlignVCenter,
                   QString::number(m_label));
    }
}

void TouchDial::mousePressEvent(QMouseEvent *event) { mouseMoveEvent(event); }

void TouchDial::mouseMoveEvent(QMouseEvent *event) {
    if (m_readOnly) return;

    const int w = width();
    const int h = height();

    const int d = qMin(w, h);
    const int r = d / 2;

    const int xoff = (w - d) / 2;
    const int yoff = (h - d) / 2;

    QPoint posF = event->pos();
    double x = (posF.x() - xoff - r);
    double y = (posF.y() - yoff - r);

    double angle = atan(y / x);

    if (x < 0.0)
        angle += M_PI;
    else if (y < 0.0)
        angle += 2 * M_PI;
    else
        angle -= M_PI / 4;

    double value = angleToValue(angle) * (m_maximumValue - m_minimumValue) +
                   m_minimumValue;

    // Correct dead zone
    if (angle < (3.0 * M_PI) / 4.0 && angle > M_PI / 2.0 && value > 0.5)
        value = m_minimumValue;

    setValue(value);
}

inline double TouchDial::valueToAngle(const double &value) {
    return (M_PI / 4.0) * (6.0 * value - 1.0) + M_PI;
}

inline double TouchDial::angleToValue(const double &angle) {
    double value = (4.0 * angle - 3 * M_PI) / (6.0 * M_PI);
    if (value < 0.0) value += 1.5;
    return value;
}