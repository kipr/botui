#include "KovanDevice.h"

#include "BatteryLevelProvider.h"
#include "NetworkingProvider.h"
#include "CommunicationProvider.h"
#include "FilesystemProvider.h"
#include "KovanProgramsItemModel.h"
#include "EasyDeviceCommunicationProvider.h"
#include "Connman.h"
#include "KissCompileProvider.h"

#include "BuildOptions.h"

#include <QStandardItemModel>
#include <QFileSystemModel>

#include <QDir>
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
	: QStandardItem(name), m_name(name)
{
	setSizeHint(QSize(0, 30));
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
		if(ProgramItem::programitem_cast(item(i))->name() == name) {
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
	TinyArchiveFile fileReader((QDir::homePath() + "/" + name).toStdString());
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

Kovan::Device::Device()
	: m_filesystemProvider(new Kovan::FilesystemProvider()),
	m_compileProvider(new KissCompileProvider()),
	m_communicationProviders(CommunicationProviderList()
		<< new EasyDeviceCommunicationProvider(this)),
	m_networkingProvider(new Kovan::Connman()),
	m_batteryLevelProvider(new Kovan::BatteryLevelProvider())
{
	m_networkingProvider->setup();
}

Kovan::Device::~Device()
{
	delete m_filesystemProvider;
	delete m_compileProvider;
	qDeleteAll(m_communicationProviders);
	delete m_batteryLevelProvider;
	delete m_networkingProvider;
}

QString Kovan::Device::name() const
{
	return "Kovan";
}

QString Kovan::Device::version() const
{
	return "0.1a";
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