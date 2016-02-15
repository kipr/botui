#include "WallabyBatteryProvider.h"

#include <wallaby/general.h>
#include <wallaby/battery.hpp>

#include <QDebug>

const float Wallaby::BatteryLevelProvider::batteryLevel() const
{
#ifndef NOT_A_WALLABY
  publish();
#endif
  
  const float batteryLevel = Battery::powerLevel(m_batteryType);
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

void Wallaby::BatteryLevelProvider::setBatteryType(const int type)
{
  m_batteryType = type;
  qDebug() << "Battery type is now " << type;
}

int Wallaby::BatteryLevelProvider::batteryType() const
{
  return m_batteryType;
}