#include "WombatBatteryProvider.h"

#include <kipr/battery/battery.hpp>

using namespace kipr::battery;

const float Wombat::BatteryLevelProvider::batteryLevel() const {

    const float batteryLevel = Battery::powerLevel(m_batteryType);
    float perc = batteryLevel;
    if (perc > 1.0f) perc = 1.0f;
    if (perc < 0.0f) perc = 0.0f;
    return perc;
}

const bool Wombat::BatteryLevelProvider::isCharging() const {

    return Battery::isCharging();
}

void Wombat::BatteryLevelProvider::setBatteryType(const int type) {
    m_batteryType = type;
}

int Wombat::BatteryLevelProvider::batteryType() const { return m_batteryType; }

void Wombat::BatteryLevelProvider::setWarningThresh(const float thresh) {
    m_warningThresh = thresh;
}

float Wombat::BatteryLevelProvider::warningThresh() const {
    return m_warningThresh;
}