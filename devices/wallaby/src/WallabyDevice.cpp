#include "WallabyDevice.h"

#include "WallabyBatteryProvider.h"
#include "WallabySettingsProvider.h"
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
  m_batteryLevelWarningThresh(0.1f), // TODO: settings
  m_version(getVersionNum()),
  m_id(getId())
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
  killTimer(m_timerId);
  delete m_compileProvider;
  delete m_batteryLevelProvider;
  delete m_settingsProvider;
}

QString Wallaby::Device::name() const
{
  return tr("Wallaby");
}

QString Wallaby::Device::version() const
{
  return m_version;
}

QString Wallaby::Device::id() const
{
  return m_id;
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
  return 0;
}

void Wallaby::Device::settingsChanged()
{
  const int type = m_settingsProvider->value("battery_type", 0).toInt();
  ((Wallaby::BatteryLevelProvider *)m_batteryLevelProvider)->setBatteryType(type);
}

void Wallaby::Device::timerEvent(QTimerEvent *event)
{
  const float batteryLevel = m_batteryLevelProvider->batteryLevel();

  static const double WAV_CYCLE_TIME = 5.0;
  static auto last_warn_time = std::chrono::system_clock::now();

  if (batteryLevel < m_batteryLevelWarningThresh)
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
