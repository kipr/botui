#include "PlotWidget.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>

#define GOLDEN_RATIO 2.39996

struct Plot {
    Plot(const quint32 &res, const QColor &color)
        : values(new double[res]), color(color) {}

    ~Plot() { delete[] values; }

    double *values;
    QColor color;
};

PlotWidget::PlotWidget(QWidget *parent)
    : QWidget(parent), m_resolution(120), m_location(0) {}

PlotWidget::~PlotWidget() {
    while (!m_plots.isEmpty())
        delete m_plots.takeFirst();
}

PlotHandle PlotWidget::addPlot(const QColor &color) {
    m_plots.append(new Plot(m_resolution, color));
    return m_plots.size() - 1;
}

void PlotWidget::removePlot(const PlotHandle &handle) {
    delete m_plots[handle];
    m_plots.replace(handle, 0);
}

void PlotWidget::push(const PlotHandle &handle, const double &value) {
    double safeValue = value;
    if (safeValue > 1.0)
        safeValue = 1.0;
    else if (safeValue < -1.0)
        safeValue = -1.0;
    m_plots[handle]->values[m_location] = safeValue;
}

void PlotWidget::inc() {
    ++m_location;
    m_location %= m_resolution;
    update();
}

#define MOUSE_TO_ASD                                                           \
    ((height() - event->pos().y()) / (double)height() * 2.0 - 1.0)

void PlotWidget::mousePressEvent(QMouseEvent *event) {
    emit mouseEvent(MOUSE_TO_ASD);
}

void PlotWidget::mouseMoveEvent(QMouseEvent *event) {
    emit mouseEvent(MOUSE_TO_ASD);
}

void PlotWidget::mouseReleaseEvent(QMouseEvent *event) {
    emit mouseEvent(MOUSE_TO_ASD);
}

void PlotWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(0, 0, width() - 1, height() - 1, Qt::transparent);
    const double max = (double)height() / 2.1;
    const double widthRatio = (double)width() / (double)m_resolution;
    foreach (Plot *plot, m_plots) {
        painter.setPen(QPen(plot->color, 2));
        painter.setBrush(plot->color);
        double oldX = 0.0;
        double oldY = max - plot->values[0] * max;
        for (quint32 i = 0; i < m_resolution - 1; ++i) {
            const int off = (i + m_location) % m_resolution;
            double newX = i * widthRatio;
            double newY = height() / 2.0 - plot->values[off] * max;
            painter.drawLine(oldX, oldY, newX, newY);
            oldX = newX;
            oldY = newY;
        }
    }

    // painter.fillRect((m_location - 1) * widthRatio, 0, widthRatio, height(),
    // Qt::black);
}
