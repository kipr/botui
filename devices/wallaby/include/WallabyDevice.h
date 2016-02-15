#ifndef _WALLABY_DEVICE_H_
#define _WALLABY_DEVICE_H_

#include "Device.h"

namespace Wallaby
{  
  class Device : public ::Device
  {
  Q_OBJECT
  public:
    Device();
    ~Device();
    
    virtual QString name() const;
    virtual QString version() const;
    virtual QString id() const;
    virtual bool isTouchscreen() const;
    
    virtual CompileProvider *compileProvider() const;
    virtual BatteryLevelProvider *batteryLevelProvider() const;
    virtual SettingsProvider *settingsProvider() const;
    virtual ButtonProvider *buttonProvider() const;
    
  private slots:
    void settingsChanged();
    
  private:
    QString getId() const;
    
    CompileProvider *m_compileProvider;
    BatteryLevelProvider *m_batteryLevelProvider;
    SettingsProvider *m_settingsProvider;
    QString m_version;
    QString m_id;
  };
}

#endif
