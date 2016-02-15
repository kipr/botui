#ifndef _WALLABY_BATTERY_PROVIDER_H
#define _WALLABY_BATTERY_PROVIDER_H

#include "BatteryLevelProvider.h"

#include <QString>

namespace Wallaby
{
  class BatteryLevelProvider : public ::BatteryLevelProvider
  {
  public:
    virtual const float batteryLevel() const;
    virtual const bool isCharging() const;
    
    void setBatteryType(const int type);
    int batteryType() const;
    
  private:
    int m_batteryType;
  };
}

#endif