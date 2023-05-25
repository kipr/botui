#ifndef _BATTERYLEVELPROVIDER_H_
#define _BATTERYLEVELPROVIDER_H_

class BatteryLevelProvider {
  public:
    virtual ~BatteryLevelProvider();

    virtual const float batteryLevel() const = 0;
    virtual const bool isCharging() const = 0;
};

#endif
