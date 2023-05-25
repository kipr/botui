#ifndef _PIDTUNERWIDGET_H_
#define _PIDTUNERWIDGET_H_

#include "PlotWidget.h"
#include "StandardWidget.h"
#include <QTimer>

namespace Ui {
class PidTunerWidget;
}

class PidTunerWidget : public StandardWidget {
    Q_OBJECT
  public:
    PidTunerWidget(Device *device, QWidget *parent = 0);
    ~PidTunerWidget();

  private slots:
    void coeffChanged();
    void motorChanged();

    void update();
    void mouseEvent(double y);

  private:
    Ui::PidTunerWidget *ui;
    PlotHandle m_feedback;
    PlotHandle m_setpoint;

    double m_setpointVal;
    double m_feedbackVal;
    int m_position_1;
    double m_vel_1;

    double getFeedbackValue();
    void updatePids();
};

#endif
