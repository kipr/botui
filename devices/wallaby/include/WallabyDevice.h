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
    virtual QString serial() const;
    virtual bool isTouchscreen() const;
    
    virtual CompileProvider *compileProvider() const;
    virtual BatteryLevelProvider *batteryLevelProvider() const;
    virtual SettingsProvider *settingsProvider() const;
    virtual ButtonProvider *buttonProvider() const;
    
  private slots:
    void settingsChanged();

  protected:
    void timerEvent(QTimerEvent *event);

  private:
    QString getId() const;
    QString getSerial() const;
    
    CompileProvider *m_compileProvider;
    BatteryLevelProvider *m_batteryLevelProvider;
    SettingsProvider *m_settingsProvider;
    ButtonProvider *m_buttonProvider;
    QString m_version;
    QString m_id;
    QString m_serial;
    int m_timerId;
  };
}

#endif
