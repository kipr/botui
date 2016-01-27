#include "WallabyBatteryProvider.h"

#include <wallaby/general.h>
#include <wallaby/battery.hpp>

const float Wallaby::BatteryLevelProvider::batteryLevel() const
{
#ifndef NOT_A_WALLABY
  publish();
#endif

  const float batteryLevel = Battery::powerLevel();
  float perc = batteryLevel;
  if (perc > 1.0f) perc = 1.0f;
  if (perc < 0.0f) perc = 0.0f;
  return perc;
}


const bool Wallaby::BatteryLevelProvider::isCharging() const
{
#ifndef NOT_A_WALLABY
  publish();
#endif

  return Battery::isCharging();
}
