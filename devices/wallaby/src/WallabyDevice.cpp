#include "WallabyDevice.h"

#include "WallabyBatteryProvider.h"
#include "WallabySettingsProvider.h"
#include "WallabyButtonProvider.h"
#include "KissCompileProvider.h"

#include <wallaby/wallaby.hpp>

#ifdef ENABLE_DBUS_SUPPORT
#include <QDBusConnection>
#endif

#include <QFileInfo>
#include <QProcess>
#include <QDebug>

#include <iostream>
#include <chrono>

#ifdef Q_OS_MAC
#define NOT_A_WALLABY
#endif


QString getVersionNum()
{
	QFile file("/usr/share/kipr/board_fw_version.txt");

	if(!file.open(QIODevice::ReadOnly)) return QString("??");

	QTextStream in(&file);

    QString line = in.readLine();

	file.close();

	return line;
}

Wallaby::Device::Device()
  : m_compileProvider(new KissCompileProvider(this)),
  m_batteryLevelProvider(new Wallaby::BatteryLevelProvider()),
  m_settingsProvider(new Wallaby::SettingsProvider()),
  m_buttonProvider(new Wallaby::ButtonProvider()),
  m_version(getVersionNum()),
  m_id(getId()),
  m_serial(getSerial())
{
  m_compileProvider->setBinariesPath("/wallaby/bin");
  connect(m_settingsProvider, SIGNAL(settingsChanged()), SLOT(settingsChanged()));
#ifndef NOT_A_WALLABY
  halt();
#endif
  set_auto_publish(0);

  m_timerId = startTimer(1000);

  // load settings
  settingsChanged();
}

Wallaby::Device::~Device()
{
  if(m_timerId > 0)
    killTimer(m_timerId);
  delete m_compileProvider;
  delete m_batteryLevelProvider;
  delete m_settingsProvider;
}

QString Wallaby::Device::name() const
{
  return tr("Wombat");
}

QString Wallaby::Device::version() const
{
  return m_version;
}

QString Wallaby::Device::id() const
{
  return m_id;
}

QString Wallaby::Device::serial() const
{
  return m_serial;
}

bool Wallaby::Device::isTouchscreen() const
{
#ifdef NOT_A_WALLABY
  return false;
#else
  return true;
#endif
}

CompileProvider *Wallaby::Device::compileProvider() const
{
  return m_compileProvider;
}

BatteryLevelProvider *Wallaby::Device::batteryLevelProvider() const
{
  return m_batteryLevelProvider;
}

SettingsProvider *Wallaby::Device::settingsProvider() const
{
  return m_settingsProvider;
}

ButtonProvider *Wallaby::Device::buttonProvider() const
{
  return m_buttonProvider;
}

// TODO: Device shouldn't be responsible for doing this
// TODO: Connect setting provider's signal to a battery provider slot that will load settings
void Wallaby::Device::settingsChanged()
{
  const int type = m_settingsProvider->value("battery_type", 0).toInt();
  const float thresh = m_settingsProvider->value("battery_warning_thresh", 0.1f).toFloat();
  const bool enabled = m_settingsProvider->value("battery_warning_enabled", true).toBool();
  
  Wallaby::BatteryLevelProvider *wblProvider = (Wallaby::BatteryLevelProvider *)m_batteryLevelProvider;
  wblProvider->setBatteryType(type);
  wblProvider->setWarningThresh(thresh);
  
  if(m_timerId > 0 && !enabled)
  {
    killTimer(m_timerId);
    m_timerId = 0;
  }
  else if(m_timerId <= 0 && enabled)
    m_timerId = startTimer(1000);
}

void Wallaby::Device::timerEvent(QTimerEvent *event)
{
  const float batteryLevel = m_batteryLevelProvider->batteryLevel();
  const float warningThresh = ((Wallaby::BatteryLevelProvider *)m_batteryLevelProvider)->warningThresh();
  
  static const double WAV_CYCLE_TIME = 5.0;
  static auto last_warn_time = std::chrono::system_clock::now();

  if (batteryLevel < warningThresh)
  {
    auto now = std::chrono::system_clock::now();
    if (std::chrono::duration<double>(now-last_warn_time).count() > WAV_CYCLE_TIME)	
    {
      std::cout << "Low battery!" << std::endl;
      system("aplay /usr/share/botui/turn_off_wallaby.wav &");
      last_warn_time = now;
    }
  }
}

QString Wallaby::Device::getId() const
{
  const QFileInfo getIdScript("/usr/bin/wallaby_get_id.sh");
  if(!getIdScript.exists() || !getIdScript.isFile()) {
    qWarning() << "wallaby_get_id script does not exist";
    return QString();
  }
  
  QProcess proc;
  proc.start("/bin/sh", QStringList() << getIdScript.absoluteFilePath());
  if(proc.waitForFinished(5000))
    return proc.readAllStandardOutput();
  else
  {
    qWarning() << "Failed to get wallaby id";
    return QString();
  }
}

QString Wallaby::Device::getSerial() const
  {
    const QFileInfo getIdScript("/usr/bin/wallaby_get_serial.sh");
    if(!getIdScript.exists() || !getIdScript.isFile()) {
      qWarning() << "wallaby_get_serial script does not exist";
      return QString();
    }
  QProcess proc;
  proc.start("/bin/sh", QStringList() << getIdScript.absoluteFilePath());
  if(proc.waitForFinished(5000))
    return proc.readAllStandardOutput();
  else
  {
    qWarning() << "Failed to get wallaby serial";
    return QString();
  }
}
