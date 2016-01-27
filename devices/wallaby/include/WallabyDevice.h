#ifndef _WALLABY_DEVICE_H_
#define _WALLABY_DEVICE_H_

#include "Device.h"

namespace Wallaby
{  
  class Device : public ::Device
  {
  public:
    Device();
    ~Device();
    
    virtual QString name() const;
    virtual QString version() const;
    virtual bool isTouchscreen() const;
    
    virtual CompileProvider *compileProvider() const;
    virtual BatteryLevelProvider *batteryLevelProvider() const;
    virtual SettingsProvider *settingsProvider() const;
    virtual ButtonProvider *buttonProvider() const;
    
  private:
    CompileProvider *m_compileProvider;
    BatteryLevelProvider *m_batteryLevelProvider;
    SettingsProvider *m_settingsProvider;
    QString m_version;

  };
}

#endif
