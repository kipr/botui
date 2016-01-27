#ifndef _WALLABY_BATTERY_PROVIDER_H
#define _WALLABY_BATTERY_PROVIDER_H

#include "BatteryLevelProvider.h"

namespace Wallaby
{
  class BatteryLevelProvider : public ::BatteryLevelProvider
  {
  public:
    virtual const float batteryLevel() const;
    virtual const bool isCharging() const;
  };
}

#endif