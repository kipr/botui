#ifndef _BATTERYWIDGET_H_
#define _BATTERYWIDGET_H_

#include <BatteryLevelProvider.h>
#include <QWidget>

class BatteryLevelProvider;

class BatteryWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(BatteryLevelProvider *batteryLevelProvider READ
                   batteryLevelProvider WRITE setBatteryLevelProvider)
  public:
    BatteryWidget(QWidget *parent = 0);
    BatteryWidget(BatteryLevelProvider *batteryLevelProvider,
                  QWidget *parent = 0);

    void setBatteryLevelProvider(BatteryLevelProvider *batteryLevelProvider);
    BatteryLevelProvider *batteryLevelProvider() const;

  protected:
    void paintEvent(QPaintEvent *event);

  private:
    void constrain();

    void startTimer();

    BatteryLevelProvider *m_batteryLevelProvider;
};

#endif
