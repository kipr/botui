#include "KovanDevice.h"

#include "BatteryLevelProvider.h"
#include "NetworkingProvider.h"
#include "CommunicationProvider.h"
#include "FilesystemProvider.h"
#include "SettingsProvider.h"
#include "KovanProgramsItemModel.h"
#include "EasyDeviceCommunicationProvider.h"
#include "Connman.h"
#include "KissCompileProvider.h"
#include "KovanButtonProvider.h"

#include "BuildOptions.h"

#include "RootController.h"
#include "KeyboardDialog.h"

#include <QStandardItemModel>
#include <QFileSystemModel>
#include <QMessageBox>
#include <kiss-compiler/PlatformHintsManager.h>

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
		
		virtual const bool setProgram(const QString& name, const TinyArchive *archive);
		virtual const bool deleteProgram(const QString& name);
		virtual TinyArchive *program(const QString& name) const;
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
	return Battery::powerLevel();
}

const float Kovan::BatteryLevelProvider::batteryLevelMin() const
{
	return 600.0; // That's bogus
}

const float Kovan::BatteryLevelProvider::batteryLevelMax() const
{
	return 800.0; // That's also bogus
}

const bool Kovan::BatteryLevelProvider::isCharging() const
{
	return batteryLevel() > 1000.0;
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
	
	connect(m_filesystemProvider, SIGNAL(programUpdated(QString, const TinyArchive *)), SLOT(programUpdated(QString)));
	connect(m_filesystemProvider, SIGNAL(programDeleted(QString)), SLOT(programDeleted(QString)));
}

void Kovan::ProgramsItemModel::programUpdated(const QString& name)
{
	qDebug() << name;
	const int count = rowCount();
	for(int i = 0; i < count; ++i) {
		Kovan::ProgramItem *item = ProgramItem::programitem_cast(QStandardItemModel::item(i));
		if(item && item->name() == name) {
			insertRow(0, takeItem(i));
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
			delete takeItem(i);
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

const bool Kovan::FilesystemProvider::setProgram(const QString& name, const TinyArchive *archive)
{
	qDebug() << "Set Program Called";
	TinyArchiveFile fileWriter(pathForProgram(name).toStdString());
	const bool ret = fileWriter.write(archive);
	if(ret) emit programUpdated(name, archive);
	qDebug() << "Program Updated?" << ret;
	return ret;
}

const bool Kovan::FilesystemProvider::deleteProgram(const QString& name)
{
	const bool ret = QFile::remove(pathForProgram(name));
	if(ret) emit programDeleted(name);
	return ret;
}

TinyArchive *Kovan::FilesystemProvider::program(const QString& name) const
{
	TinyArchiveFile fileReader(pathForProgram(name).toStdString());
	return fileReader.read();
}

const QStringList Kovan::FilesystemProvider::programs() const
{
	QStringList ret;
	const QFileInfoList& entries = programDir().entryInfoList(QStringList() << "*.kissproj",
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
	return programDir().path() + "/" + program + ".kissproj";
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
	m_networkingProvider(new Kovan::Connman(this)),
	m_batteryLevelProvider(new Kovan::BatteryLevelProvider()),
	m_settingsProvider(new Kovan::SettingsProvider()),
	m_buttonProvider(new Kovan::ButtonProvider(this))
{
	m_networkingProvider->setup();
	
	
	// TODO: Move these
	FlagMap flagMap;
	flagMap["C_FLAGS"] = "-Wall -include kovan/kovan.h";
	flagMap["CXX_FLAGS"] = "-Wall -include kovan/kovan.hpp";
	flagMap["LD_FLAGS"] = "-lkovan";
	PlatformHintsManager::ref().setPlatformHints(name(), flagMap);
}

Kovan::Device::~Device()
{
	delete m_filesystemProvider;
	delete m_compileProvider;
	qDeleteAll(m_communicationProviders);
	delete m_batteryLevelProvider;
	delete m_networkingProvider;
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

NetworkingProvider *Kovan::Device::networkingProvider() const
{
	return m_networkingProvider;
}

BatteryLevelProvider *Kovan::Device::batteryLevelProvider() const
{
	return m_batteryLevelProvider;
}

PackageManagerProvider *Kovan::Device::packageManagerProvider() const
{
	return 0;
}

SettingsProvider *Kovan::Device::settingsProvider() const
{
	return m_settingsProvider;
}

ButtonProvider *Kovan::Device::buttonProvider() const
{
	return m_buttonProvider;
}

QString Kovan::Device::networkingProviderNeedsPasswordOfType(NetworkingProvider *networkingProvider, const QString& type)
{
	KeyboardDialog keyboard("Password (" + type + ")");
	const int ret = RootController::ref().presentDialog(&keyboard);
	return ret == QDialog::Accepted ? keyboard.input() : "";
}

QString Kovan::Device::networkingProviderNeedsNetworkName(NetworkingProvider *networkingProvider)
{
	KeyboardDialog keyboard("Network Name");
	const int ret = RootController::ref().presentDialog(&keyboard);
	return ret == QDialog::Accepted ? keyboard.input() : "";
}

void Kovan::Device::networkingProviderReportedError(NetworkingProvider *networkingProvider, const QString& message)
{
	QMessageBox::critical(0, tr("Networking Error"), message);
}