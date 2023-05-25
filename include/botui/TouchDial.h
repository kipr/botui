#ifndef _TOUCHDIAL_H_
#define _TOUCHDIAL_H_

#include <QWidget>

class TouchDial : public QWidget {
    Q_OBJECT
  public:
    TouchDial(QWidget *parent = 0);
    ~TouchDial();

    Q_PROPERTY(double minimumValue READ minimumValue WRITE setMinimumValue)
    void setMinimumValue(const double &minimumValue);
    const double &minimumValue() const;

    Q_PROPERTY(double maximumValue READ maximumValue WRITE setMaximumValue)
    void setMaximumValue(const double &maximumValue);
    const double &maximumValue() const;

    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    void setValue(const double &value);
    const double &value() const;

    Q_PROPERTY(quint16 label READ label WRITE setLabel)
    void setLabel(const quint16 &label);
    const quint16 &label() const;

    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly)
    void setReadOnly(const bool &readOnly);
    const bool &readOnly() const;

  signals:
    void valueChanged(double fraction);

  protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

  private:
    double valueToAngle(const double &value);
    double angleToValue(const double &angle);

    double m_minimumValue;
    double m_maximumValue;
    double m_value;

    quint16 m_label;

    bool m_readOnly;
};

#endif
