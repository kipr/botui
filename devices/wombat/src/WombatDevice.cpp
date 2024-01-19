#include "WombatDevice.h"

#include "WombatBatteryProvider.h"
#include "WombatSettingsProvider.h"
#include "WombatButtonProvider.h"
#include "KissCompileProvider.h"

#include <kipr/wombat.h>

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

Wombat::Device::Device()
  : m_compileProvider(new KissCompileProvider(this)),
  m_batteryLevelProvider(new Wombat::BatteryLevelProvider()),
  m_settingsProvider(new Wombat::SettingsProvider()),
  m_buttonProvider(new Wombat::ButtonProvider()),
  m_version(getVersionNum()),
  m_id(getId()),
  m_serial(getSerial())
{
  m_compileProvider->setBinariesPath("/wallaby/bin");
  connect(m_settingsProvider, SIGNAL(settingsChanged()), SLOT(settingsChanged()));

  m_timerId = startTimer(1000);

  // load settings
  settingsChanged();
}

Wombat::Device::~Device()
{
  if(m_timerId > 0)
    killTimer(m_timerId);
  delete m_compileProvider;
  delete m_batteryLevelProvider;
  delete m_settingsProvider;
}

QString Wombat::Device::name() const
{
  return tr("Wombat");
}

QString Wombat::Device::version() const
{
  return m_version;
}

QString Wombat::Device::id() const
{
  return m_id;
}

QString Wombat::Device::serial() const
{
  return m_serial;
}

bool Wombat::Device::isTouchscreen() const
{
#ifdef NOT_A_WALLABY
  return false;
#else
  return true;
#endif
}

CompileProvider *Wombat::Device::compileProvider() const
{
  return m_compileProvider;
}

BatteryLevelProvider *Wombat::Device::batteryLevelProvider() const
{
  return m_batteryLevelProvider;
}

SettingsProvider *Wombat::Device::settingsProvider() const
{
  return m_settingsProvider;
}

ButtonProvider *Wombat::Device::buttonProvider() const
{
  return m_buttonProvider;
}

// TODO: Device shouldn't be responsible for doing this
// TODO: Connect setting provider's signal to a battery provider slot that will load settings
void Wombat::Device::settingsChanged()
{
  const int type = m_settingsProvider->value("battery_type", 0).toInt();
  const float thresh = m_settingsProvider->value("battery_warning_thresh", 0.1f).toFloat();
  const bool enabled = m_settingsProvider->value("battery_warning_enabled", true).toBool();
  
  Wombat::BatteryLevelProvider *wblProvider = (Wombat::BatteryLevelProvider *)m_batteryLevelProvider;
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

void Wombat::Device::timerEvent(QTimerEvent *event)
{
  const float batteryLevel = m_batteryLevelProvider->batteryLevel();
  const float warningThresh = ((Wombat::BatteryLevelProvider *)m_batteryLevelProvider)->warningThresh();
  
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

QString Wombat::Device::getId() const
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

QString Wombat::Device::getSerial() const
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
