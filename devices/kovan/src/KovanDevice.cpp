#include "KovanDevice.h"

#include "BatteryLevelProvider.h"
#include "CommunicationProvider.h"
#include "FilesystemProvider.h"
#include "SettingsProvider.h"
#include "KovanProgramsItemModel.h"
#include "EasyDeviceCommunicationProvider.h"
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
	
	class ProgramItem : public QStandardItem
	{
	public:
		ProgramItem(const QString& name);
		
		const QString& name() const;
		
		template<typename T>
		static ProgramItem *programitem_cast(T *t)
		{
			return dynamic_cast<ProgramItem *>(t);
		}
		
	private:
		const QString m_name;
	};
	
	class FilesystemProvider : public ::FilesystemProvider
	{
	public:
		FilesystemProvider();
		~FilesystemProvider();
		
		virtual const bool setProgram(const QString& name, const Kiss::KarPtr& program);
		virtual const bool deleteProgram(const QString& name);
		virtual Kiss::KarPtr program(const QString& name) const;
		virtual const QStringList programs() const;
		virtual ::ProgramsItemModel *programsItemModel() const;
		
		friend class ProgramsItemModel;
	private:
		QDir programDir() const;
		QString pathForProgram(const QString& program) const;
		
		Kovan::ProgramsItemModel *m_programsItemModel;
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
	//publish();
	return Battery::powerLevel();
}

const float Kovan::BatteryLevelProvider::batteryLevelMin() const
{
	return 500.0; // That's bogus
}

const float Kovan::BatteryLevelProvider::batteryLevelMax() const
{
	return 625.0; // That's also bogus
}

const bool Kovan::BatteryLevelProvider::isCharging() const
{
	return batteryLevel() > batteryLevelMax();
}

Kovan::ProgramItem::ProgramItem(const QString& name)
	: QStandardItem(QIcon(":/icons/bricks.png"), name), m_name(name)
{
	setSizeHint(QSize(0, 30));
	setEditable(false);
}

const QString& Kovan::ProgramItem::name() const
{
	return m_name;
}

Kovan::ProgramsItemModel::ProgramsItemModel(Kovan::FilesystemProvider *filesystemProvider, QObject *parent)
	: ::ProgramsItemModel(parent), m_filesystemProvider(filesystemProvider)
{
	foreach(const QString& program, m_filesystemProvider->programs()) appendRow(new ProgramItem(program));
	
	connect(m_filesystemProvider, SIGNAL(programUpdated(QString, const Kiss::KarPtr&)), SLOT(programUpdated(QString)));
	connect(m_filesystemProvider, SIGNAL(programDeleted(QString)), SLOT(programDeleted(QString)));
}

void Kovan::ProgramsItemModel::programUpdated(const QString& name)
{
	qDebug() << name;
	const int count = rowCount();
	for(int i = 0; i < count; ++i) {
		Kovan::ProgramItem *item = ProgramItem::programitem_cast(QStandardItemModel::item(i));
		if(item && item->name() == name) {
			insertRow(0, takeRow(i)[0]);
			return;
		}
	}
	insertRow(0, new ProgramItem(name));
}

void Kovan::ProgramsItemModel::programDeleted(const QString& name)
{
	const int count = rowCount();
	for(int i = 0; i < count; ++i) {
		ProgramItem *item = ProgramItem::programitem_cast(ProgramsItemModel::item(i));
		if(item && item->name() == name) {
			qDeleteAll(takeRow(i));
			return;
		}
	}
}

Kovan::FilesystemProvider::FilesystemProvider()
	: m_programsItemModel(new Kovan::ProgramsItemModel(this))
{
	
}

Kovan::FilesystemProvider::~FilesystemProvider()
{
	delete m_programsItemModel;
}

const bool Kovan::FilesystemProvider::setProgram(const QString& name, const Kiss::KarPtr& program)
{
	qDebug() << "Set Program Called";
	QFile out(pathForProgram(name));
	if(!out.open(QIODevice::WriteOnly)) return false;
	QDataStream stream(&out);
	stream << *program.data();
	out.close();
	emit programUpdated(name, program);
	qDebug() << "Program" << name << "Updated";
	return true;
}

const bool Kovan::FilesystemProvider::deleteProgram(const QString& name)
{
	const bool ret = QFile::remove(pathForProgram(name));
	if(ret) emit programDeleted(name);
	return ret;
}

Kiss::KarPtr Kovan::FilesystemProvider::program(const QString& name) const
{
	QFile in(pathForProgram(name));
	if(!in.open(QIODevice::ReadOnly)) return Kiss::KarPtr();
	QDataStream stream(&in);
	Kiss::Kar *program = new Kiss::Kar();
	stream >> (*program);
	in.close();
	return Kiss::KarPtr(program);
}

const QStringList Kovan::FilesystemProvider::programs() const
{
	QStringList ret;
	const QFileInfoList& entries = programDir().entryInfoList(QStringList() << "*.kissar",
		QDir::NoDot | QDir::NoDotDot | QDir::Files);
	foreach(const QFileInfo& info, entries) ret << info.completeBaseName();
	return ret;
}

::ProgramsItemModel *Kovan::FilesystemProvider::programsItemModel() const
{
	return m_programsItemModel;
}

QDir Kovan::FilesystemProvider::programDir() const
{
	QDir home = QDir::home();
	home.mkdir("programs");
	home.cd("programs");
	return home;
}

QString Kovan::FilesystemProvider::pathForProgram(const QString& program) const
{
	return programDir().path() + "/" + program + ".kissar";
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
	: m_filesystemProvider(new Kovan::FilesystemProvider()),
	m_compileProvider(new KissCompileProvider(this)),
	m_communicationProviders(CommunicationProviderList()
		<< new EasyDeviceCommunicationProvider(this)),
	m_batteryLevelProvider(new Kovan::BatteryLevelProvider()),
	m_settingsProvider(new Kovan::SettingsProvider()),
	m_buttonProvider(new Kovan::ButtonProvider(this))
{
}

Kovan::Device::~Device()
{
	delete m_filesystemProvider;
	delete m_compileProvider;
	qDeleteAll(m_communicationProviders);
	delete m_batteryLevelProvider;
	delete m_settingsProvider;
	delete m_buttonProvider;
}

QString Kovan::Device::name() const
{
	return "Kovan";
}

QString Kovan::Device::version() const
{
	return "0.1a";
}

bool Kovan::Device::isTouchscreen() const
{
	return false;
}

FilesystemProvider *Kovan::Device::filesystemProvider() const
{
	return m_filesystemProvider;
}

CompileProvider *Kovan::Device::compileProvider() const
{
	return m_compileProvider;
}

CommunicationProviderList Kovan::Device::communicationProviders() const
{
	return m_communicationProviders;
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
