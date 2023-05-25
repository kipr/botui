#ifndef _QHSVPICKER_H_
#define _QHSVPICKER_H_

#include <QWidget>

class QTimer;

class QHsvPicker : public QWidget {
    Q_OBJECT
  public:
    QHsvPicker(QWidget *parent = 0);
    ~QHsvPicker();

    Q_PROPERTY(QColor max READ max WRITE setMax)
    void setMax(const QColor &max);
    const QColor &max() const;

    Q_PROPERTY(QColor min READ min WRITE setMin)
    void setMin(const QColor &min);
    const QColor &min() const;

    Q_PROPERTY(unsigned dragHintRadius READ dragHintRadius)
    unsigned dragHintRadius() const;

    Q_PROPERTY(unsigned slideTimeout READ slideTimeout WRITE setSlideTimeout)
    void setSlideTimeout(const unsigned &slideTimeout);
    const unsigned &slideTimeout() const;

  signals:
    void minChanged(const QColor &min);
    void maxChanged(const QColor &min);

  protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

  private slots:
    void slide();

  private:
    enum ActiveDragHint {
        None = 0,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        All
    };

    QRect selectionS() const;
    QRect selectionV() const;

    bool isDragRange(const QPoint &mouse, const QPoint &p) const;
    void drawDragHint(QPainter &painter,
                      const QPoint &p,
                      const QColor &c,
                      const bool &active) const;

    Q_PROPERTY(int centerHue WRITE setCenterHue)
    void setCenterHue(const int &centerHue);
    void scheduleSlide();

    inline int roll(int hue) const {
        while (hue < 0)
            hue += 360;
        return hue % 360;
    }

    inline int translate(int hue) const {
        return roll(180 - m_centerHue + hue);
    }

    inline int untranslate(int hue) const {
        return roll(hue - 180 + m_centerHue);
    }

    QColor m_max;
    QColor m_min;
    int m_centerHue;

    ActiveDragHint m_activeS;
    ActiveDragHint m_activeV;

    unsigned m_slideTimeout;
    QTimer *m_slideTimer;
    QPixmap m_hsv;
};

#endif
