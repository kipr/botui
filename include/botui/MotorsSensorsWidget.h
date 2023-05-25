#ifndef _MOTORSSENSORSWIDGET_H_
#define _MOTORSSENSORSWIDGET_H_

#include "StandardWidget.h"

namespace Ui {
class MotorsSensorsWidget;
}

class MotorsSensorsWidget : public StandardWidget {
    Q_OBJECT
  public:
    MotorsSensorsWidget(Device *device, QWidget *parent = 0);
    ~MotorsSensorsWidget();

  private slots:
    void servos();
    void motors();
    void sensorGraph();
    void sensorList();
    void camera();
    void pidTuner();
    void depth();

  private:
    Ui::MotorsSensorsWidget *ui;
};

#endif
