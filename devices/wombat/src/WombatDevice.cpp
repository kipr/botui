#include "WombatDevice.h"

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

QString getCopyrightYear()
{
  QFile file("/usr/share/kipr/board_copyright_year.txt");

  if(!file.open(QIODevice::ReadOnly)) return QString("??");

  QTextStream in(&file);

    QString line = in.readLine();

  file.close();

  return line;
}

Wombat::Device::Device()
  : m_compileProvider(new KissCompileProvider(this)),
  m_settingsProvider(new Wombat::SettingsProvider()),
  m_buttonProvider(new Wombat::ButtonProvider()),
  m_version(getVersionNum()),
  m_copyrightYear(getCopyrightYear()),
  m_id(getId()),
  m_serial(getSerial())
{
  m_compileProvider->setBinariesPath("/wallaby/bin");
}

Wombat::Device::~Device()
{
  delete m_compileProvider;
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

QString Wombat::Device::copyrightYear() const
{
  return m_copyrightYear;
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

SettingsProvider *Wombat::Device::settingsProvider() const
{
  return m_settingsProvider;
}

ButtonProvider *Wombat::Device::buttonProvider() const
{
  return m_buttonProvider;
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
