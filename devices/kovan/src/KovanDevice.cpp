#include "KovanDevice.h"

#include "BatteryLevelProvider.h"
#include "CommunicationProvider.h"
#include "SettingsProvider.h"
#include "KissCompileProvider.h"
#include "KovanButtonProvider.h"

#include "BuildOptions.h"

#include "RootController.h"
#include "KeyboardDialog.h"

#include <kar/kar.hpp>

#include <QStandardItemModel>
#include <QFileSystemModel>
#include <QMessageBox>

#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QFileInfo>

#include <kovan/kovan.hpp>

#ifdef ENABLE_DBUS_SUPPORT
#include <QDBusConnection>
#endif

#include <QDebug>

#ifdef Q_OS_MAC
#define NOT_A_KOVAN
#endif

namespace Kovan
{
	class BatteryLevelProvider : public ::BatteryLevelProvider
	{
	public:
		virtual const float batteryLevel() const;
		virtual const bool isCharging() const;
	};
	
	class SettingsProvider : public ::SettingsProvider
	{
	public:
		SettingsProvider(QObject *parent = 0);
		~SettingsProvider();
		virtual void setValue(const QString& key, const QVariant& value);
		virtual QVariant value(const QString& key, const QVariant& _default = QVariant()) const;
		virtual void sync();
	private:
		QMap<QString, QVariant> m_settings;
		QFile m_settingsFile;
	};
}

const float Kovan::BatteryLevelProvider::batteryLevel() const
{
#ifndef NOT_A_KOVAN
	publish();
#endif
	float batteryLevel = Battery::powerLevel();
	float batteryLevelMin;
	float batteryLevelMax;

	if (batteryLevel > 450){
		// user has selected VANA=3.3v
		batteryLevelMin = 490;
		batteryLevelMax = 620;
	}else{
		// user has selected VANA=5.0v
		batteryLevelMin = 320;
		batteryLevelMax = 415;
	}

	return (batteryLevel - batteryLevelMin) / (batteryLevelMax - batteryLevelMin);
}


const bool Kovan::BatteryLevelProvider::isCharging() const
{
#ifndef NOT_A_KOVAN
	publish();
#endif
	return Battery::isCharging();
}

Kovan::SettingsProvider::SettingsProvider(QObject *parent)
	: ::SettingsProvider(parent),
	m_settingsFile(QDir::homePath() + "/botui_settings", this)
{
	if(!m_settingsFile.open(QIODevice::ReadOnly)) return;
	QDataStream stream(&m_settingsFile);
	stream >> m_settings;
	m_settingsFile.close();
}

Kovan::SettingsProvider::~SettingsProvider()
{
	sync();
}

void Kovan::SettingsProvider::setValue(const QString& key, const QVariant& value)
{
	m_settings[key] = value;
}

QVariant Kovan::SettingsProvider::value(const QString& key, const QVariant& _default) const
{
	return m_settings.value(key, _default);
}

void Kovan::SettingsProvider::sync()
{
	if(!m_settingsFile.open(QIODevice::WriteOnly)) return;
	QDataStream stream(&m_settingsFile);
	stream << m_settings;
	m_settingsFile.close();
}

Kovan::Device::Device()
	: m_compileProvider(new KissCompileProvider(this)),
	m_batteryLevelProvider(new Kovan::BatteryLevelProvider()),
	m_settingsProvider(new Kovan::SettingsProvider()),
	m_buttonProvider(new Kovan::ButtonProvider(this))
{
	m_compileProvider->setBinariesPath("/kovan/bin");
#ifndef NOT_A_KOVAN
	halt();
#endif
	set_auto_publish(0);
}

Kovan::Device::~Device()
{
	delete m_compileProvider;
	delete m_batteryLevelProvider;
	delete m_settingsProvider;
	delete m_buttonProvider;
}

QString Kovan::Device::name() const
{
	return tr("Link");
}

QString Kovan::Device::version() const
{
	return "2.2.1";
}

QString Kovan::Device::id() const
{
  return QString();
}

bool Kovan::Device::isTouchscreen() const
{
#ifndef NOT_A_KOVAN
	return true;
#else
	return false;
#endif
}

CompileProvider *Kovan::Device::compileProvider() const
{
	return m_compileProvider;
}

BatteryLevelProvider *Kovan::Device::batteryLevelProvider() const
{
	return m_batteryLevelProvider;
}

SettingsProvider *Kovan::Device::settingsProvider() const
{
	return m_settingsProvider;
}

ButtonProvider *Kovan::Device::buttonProvider() const
{
	return m_buttonProvider;
}
