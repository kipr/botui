#include "KovanDevice.h"

#include "BatteryLevelProvider.h"
#include "CommunicationProvider.h"
#include "DefaultArchivesManager.h"
#include "SettingsProvider.h"
#include "KissCompileProvider.h"
#include "KovanButtonProvider.h"

#include "BuildOptions.h"

#include "RootController.h"
#include "KeyboardDialog.h"

#include <kar.hpp>

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

namespace Kovan
{
	class BatteryLevelProvider : public ::BatteryLevelProvider
	{
	public:
		virtual const float batteryLevel() const;
		virtual const float batteryLevelMin() const;
		virtual const float batteryLevelMax() const;
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
	publish();
	return Battery::powerLevel();
}

const float Kovan::BatteryLevelProvider::batteryLevelMin() const
{
	return 490.0;
}

const float Kovan::BatteryLevelProvider::batteryLevelMax() const
{
	return 620.0;
}

const bool Kovan::BatteryLevelProvider::isCharging() const
{
	publish();
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
	: m_archivesManager(new DefaultArchivesManager("/kovan/archives", "/kovan/binaries", this)),
	m_compileProvider(new KissCompileProvider(this)),
	m_batteryLevelProvider(new Kovan::BatteryLevelProvider()),
	m_settingsProvider(new Kovan::SettingsProvider()),
	m_buttonProvider(new Kovan::ButtonProvider(this))
{
	m_compileProvider->setBinariesPath("/kovan/binaries");
	halt();
	set_auto_publish(0);
}

Kovan::Device::~Device()
{
	delete m_archivesManager;
	delete m_compileProvider;
	delete m_batteryLevelProvider;
	delete m_settingsProvider;
	delete m_buttonProvider;
}

QString Kovan::Device::name() const
{
	return "Link";
}

QString Kovan::Device::version() const
{
	return "1.0";
}

bool Kovan::Device::isTouchscreen() const
{
	return true;
}

ArchivesManager *Kovan::Device::archivesManager() const
{
	return m_archivesManager;
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
