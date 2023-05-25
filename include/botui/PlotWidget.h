#ifndef _PLOTWIDGET_H_
#define _PLOTWIDGET_H_

#include <QColor>
#include <QWidget>

typedef unsigned int PlotHandle;

struct Plot;

class PlotWidget : public QWidget {
    Q_OBJECT
  public:
    PlotWidget(QWidget *parent = 0);
    ~PlotWidget();

    PlotHandle addPlot(const QColor &color);
    void removePlot(const PlotHandle &handle);

    void push(const PlotHandle &handle, const double &value);
    void inc();

  signals:
    void mouseEvent(const double y);

  protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

  private:
    const quint32 m_resolution;
    QList<Plot *> m_plots;
    quint32 m_location;
};

#endif
