#include "QHsvPicker.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTimer>

QHsvPicker::QHsvPicker(QWidget *parent)
    : QWidget(parent),
      m_max(QColor::fromHsv(10, 225, 225)),
      m_min(QColor::fromHsv(350, 30, 30)),
      m_centerHue(0),
      m_activeS(QHsvPicker::None),
      m_activeV(QHsvPicker::None),
      m_slideTimeout(750),
      m_slideTimer(new QTimer(this)) {
    m_slideTimer->setSingleShot(true);
    connect(m_slideTimer, SIGNAL(timeout()), SLOT(slide()));
}

QHsvPicker::~QHsvPicker() {}

void QHsvPicker::setMax(const QColor &max) {
    m_max = max;
    scheduleSlide();
    emit maxChanged(max);
}

const QColor &QHsvPicker::max() const { return m_max; }

void QHsvPicker::setMin(const QColor &min) {
    m_min = min;
    scheduleSlide();
    emit minChanged(min);
}

const QColor &QHsvPicker::min() const { return m_min; }

unsigned QHsvPicker::dragHintRadius() const {
    return std::min(width(), height()) / 8;
}

void QHsvPicker::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    const float widthRatio = width() / 360.0f;
    // unused: const float heightRatio = height() / 255.0f;

    QLinearGradient grad;
    grad.setStart(QPoint(0, 0));
    grad.setColorAt(0.0, Qt::white);
    grad.setColorAt(1.0, Qt::black);
    const int hStep = 5;
    for (int h = 0; h < 360; h += hStep) {
        int realH = 359 - translate(h);
        grad.setFinalStop(QPoint(0, height()));
        grad.setColorAt(0.5, QColor::fromHsv(realH, 255, 255));
        painter.fillRect(
            QRectF(h * widthRatio, 0, widthRatio * hStep, height()), grad);
    }

    const QRect selS = selectionS();
    const QRect selV = selectionV();

    painter.setPen(Qt::black);
    painter.drawRect(selS.adjusted(0, 0, -1, -1));

    painter.setPen(Qt::white);
    painter.drawRect(selV.adjusted(0, 0, -1, -1));

    painter.setRenderHint(QPainter::Antialiasing);

    drawDragHint(painter, selS.topLeft(), Qt::black, m_activeS == TopLeft);
    drawDragHint(
        painter, selS.bottomRight(), Qt::black, m_activeS == BottomRight);

    drawDragHint(painter, selV.topLeft(), Qt::white, m_activeV == TopLeft);
    drawDragHint(
        painter, selV.bottomRight(), Qt::white, m_activeV == BottomRight);

    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(0, 0, width() - 1, height() - 1);
}

void QHsvPicker::mousePressEvent(QMouseEvent *event) {
    const QPoint &pos = event->pos();

    const QRect selS = selectionS();
    const QRect selV = selectionV();

    m_activeS = None;
    m_activeV = None;

    if (isDragRange(pos, selS.topLeft()))
        m_activeS = TopLeft;
    else if (isDragRange(pos, selS.bottomRight()))
        m_activeS = BottomRight;
    else if (selS.contains(pos))
        m_activeS = All;
    if (m_activeS != None) return; // Disable mutliple selection for now

    if (isDragRange(pos, selV.topLeft()))
        m_activeV = TopLeft;
    else if (isDragRange(pos, selV.bottomRight()))
        m_activeV = BottomRight;
    else if (selV.contains(pos))
        m_activeV = All;

    update();
}

void QHsvPicker::mouseMoveEvent(QMouseEvent *event) {
    if (m_activeS == None && m_activeV == None) return;

    const QPoint &p = event->pos();
    if (p.x() < 0 || p.x() > width() || p.y() < 0 || p.y() > height()) return;

    // Don't look for too long. Your eyes might start to burn.

    int newHue = 360 - translate((float)p.x() / width() * 360.0f);

    const float half = height() / 2.0f;
    int newS = p.y() / half * 255.0f;

    const static int minSize = 20;
    if (m_activeS == TopLeft) {
        if ((m_min.hue() < newHue && newHue > m_min.hue() + minSize) ||
            (m_min.hue() > newHue && newHue < m_min.hue() - minSize))
        {
            m_max.setHsv(newHue, m_max.saturation(), m_max.value());
        }
        if (newS >= m_min.saturation() + minSize && newS >= 0 && newS <= 255) {
            m_max.setHsv(m_max.hue(), newS, m_max.value());
        }
    }
    else if (m_activeS == BottomRight) {
        if ((m_max.hue() > newHue && newHue < m_max.hue() - minSize) ||
            (m_max.hue() < newHue && newHue > m_max.hue() + minSize))
        {
            m_min.setHsv(newHue, m_min.saturation(), m_min.value());
        }
        if (newS <= m_max.saturation() - minSize && newS >= 0 && newS <= 255) {
            m_min.setHsv(m_min.hue(), newS, m_min.value());
        }
    }

    int newV = 255.0f - (p.y() / half - 1.0f) * 255.0f;

    if (m_activeV == TopLeft) {
        if ((m_min.hue() < newHue && newHue > m_min.hue() + minSize) ||
            (m_min.hue() > newHue && newHue < m_min.hue() - minSize))
        {
            m_max.setHsv(newHue, m_max.saturation(), m_max.value());
        }
        if (newV >= m_min.value() + minSize && newV >= 0 && newV <= 255) {
            m_max.setHsv(m_max.hue(), m_max.saturation(), newV);
        }
    }
    else if (m_activeV == BottomRight) {
        if ((m_max.hue() > newHue && newHue < m_max.hue() - minSize) ||
            (m_max.hue() < newHue && newHue > m_max.hue() + minSize))
        {
            m_min.setHsv(newHue, m_min.saturation(), m_min.value());
        }
        if (newV <= m_max.value() - minSize && newV >= 0 && newV <= 255) {
            m_min.setHsv(m_min.hue(), m_min.saturation(), newV);
        }
    }

    update();
}

void QHsvPicker::mouseReleaseEvent(QMouseEvent *event) {
    m_activeS = m_activeV = None;

    emit minChanged(m_min);
    emit maxChanged(m_max);

    scheduleSlide();
    update();
}

void QHsvPicker::slide() {
    int newCenterH = (m_min.hue() + m_max.hue()) / 2;
    if (m_min.hue() > m_max.hue()) {
        newCenterH = m_min.hue() + (360 - m_min.hue() + m_max.hue()) / 2;
    }

    // qDebug() << "new center:" << newCenterH;
    /* QPropertyAnimation *anim = new QPropertyAnimation(this, "centerHue",
    this); anim->setDuration(500); anim->setStartValue(360 + m_centerHue);
    anim->setEndValue(360 + newCenterH);
    anim->setEasingCurve(QEasingCurve::InOutCubic);
    anim->start(); */
    setCenterHue(newCenterH);
}

QRect QHsvPicker::selectionS() const {
    const int startY = m_max.saturation();
    const int endY = m_min.saturation();

    const int selMinH = roll(m_centerHue - 180 - m_min.hue());
    const int selMaxH = roll(m_centerHue - 180 - m_max.hue());

    const float widthRatio = width() / 360.0f;
    const float heightRatio = height() / 510.0f;

    return QRect(
        QPoint(qMin(selMinH, selMaxH) * widthRatio, startY * heightRatio),
        QPoint(qMax(selMinH, selMaxH) * widthRatio, endY * heightRatio));
}

QRect QHsvPicker::selectionV() const {
    const int startY = 255 - m_max.value();
    const int endY = 255 - m_min.value();

    const int selMinH = roll(m_centerHue - 180 - m_min.hue());
    const int selMaxH = roll(m_centerHue - 180 - m_max.hue());

    const float widthRatio = width() / 360.0f;
    const float heightRatio = height() / 510.0f;

    return QRect(QPoint(qMin(selMinH, selMaxH) * widthRatio,
                        height() / 2.0f + startY * heightRatio),
                 QPoint(qMax(selMinH, selMaxH) * widthRatio,
                        height() / 2.0f + endY * heightRatio));
}

bool QHsvPicker::isDragRange(const QPoint &mouse, const QPoint &p) const {
    const int x = mouse.x() - p.x();
    const int y = mouse.y() - p.y();
    const int r = dragHintRadius();
    return x * x + y * y < r * r;
}

void QHsvPicker::drawDragHint(QPainter &painter,
                              const QPoint &p,
                              const QColor &c,
                              const bool &active) const {
    painter.setPen(c);
    int r = dragHintRadius();
    if (active) r *= 2;
    painter.setBrush(active ? QColor(c.red(), c.green(), c.blue(), 127)
                            : QColor(c.red(), c.green(), c.blue(), 50));
    painter.drawEllipse(p, r, r);
}

void QHsvPicker::setSlideTimeout(const unsigned &slideTimeout) {
    m_slideTimeout = slideTimeout;
}

const unsigned &QHsvPicker::slideTimeout() const { return m_slideTimeout; }

void QHsvPicker::setCenterHue(const int &centerHue) {
    m_centerHue = roll(centerHue);
    update();
}

void QHsvPicker::scheduleSlide() { m_slideTimer->start(m_slideTimeout); }
