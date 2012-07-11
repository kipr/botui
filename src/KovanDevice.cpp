#include "KovanDevice.h"

#include "BatteryLevelProvider.h"
#include "NetworkingProvider.h"
#include "CommunicationProvider.h"
#include "FilesystemProvider.h"
#include "KovanProgramsItemModel.h"
#include "EasyDeviceCommunicationProvider.h"

#include <QStandardItemModel>
#include <QFileSystemModel>

#include <QDir>
#include <QFileInfo>

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
	
	class NetworkItem : public QStandardItem
	{
	public:
		NetworkItem(const QString& title);
	};
	
	class NetworkItemModel : public QStandardItemModel
	{
	public:
		NetworkItemModel();
		
	protected:
		virtual void beginResetModel();
		virtual void endResetModel();
		
	private:
		void refresh();
	};
	
	class NetworkingProvider : public ::NetworkingProvider
	{
	public:
		NetworkingProvider();
		~NetworkingProvider();
		
		virtual const NetworkingProvider::NetworkState networkState() const;
		virtual void setNetworkState(const NetworkingProvider::NetworkState& state);
		
		virtual const float networkStrength() const;
		virtual const float networkStrengthMin() const;
		virtual const float networkStrengthMax() const;
		
		virtual QAbstractItemModel *networkItemModel() const;
	private:
		QAbstractItemModel *m_networkItemModel;
		NetworkingProvider::NetworkState m_state;
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
		virtual const TinyArchive *program(const QString& name) const;
		virtual const QStringList programs() const;
		virtual QAbstractItemModel *programsItemModel() const;
		
		friend class ProgramsItemModel;
	private:
		QDir programDir() const;
		QString pathForProgram(const QString& program) const;
		
		ProgramsItemModel *m_programsItemModel;
	};
}

const float Kovan::BatteryLevelProvider::batteryLevel() const
{
	return 100.0;
}

const float Kovan::BatteryLevelProvider::batteryLevelMin() const
{
	return 0.0;
}

const float Kovan::BatteryLevelProvider::batteryLevelMax() const
{
	return 100.0;
}

const bool Kovan::BatteryLevelProvider::isCharging() const
{
	return false;
}

Kovan::NetworkItem::NetworkItem(const QString& title)
	: QStandardItem(title)
{
	setSizeHint(QSize(0, 50));
}


Kovan::NetworkItemModel::NetworkItemModel()
{
	refresh();
}

void Kovan::NetworkItemModel::beginResetModel()
{
	clear();
}

void Kovan::NetworkItemModel::endResetModel()
{
	refresh();
}

void Kovan::NetworkItemModel::refresh()
{
	appendRow(new NetworkItem("One Fish"));
	appendRow(new NetworkItem("Two Fish"));
	appendRow(new NetworkItem("Red Fish"));
	appendRow(new NetworkItem("Blue Fish"));
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
	: QStandardItemModel(parent), m_filesystemProvider(filesystemProvider)
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
		if(ProgramItem::programitem_cast(item(i))->name() == name) {
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

const TinyArchive *Kovan::FilesystemProvider::program(const QString& name) const
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

QAbstractItemModel *Kovan::FilesystemProvider::programsItemModel() const
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
	: m_filesystemProvider(new Kovan::FilesystemProvider),
	m_communicationProviders(CommunicationProviderList()
		<< new EasyDeviceCommunicationProvider(this)),
	m_networkingProvider(new Kovan::NetworkingProvider),
	m_batteryLevelProvider(new Kovan::BatteryLevelProvider)
{
}

Kovan::Device::~Device()
{
	delete m_filesystemProvider;
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

Kovan::NetworkingProvider::NetworkingProvider()
	: m_networkItemModel(new Kovan::NetworkItemModel),
	m_state(NetworkOff)
{
	
}

Kovan::NetworkingProvider::~NetworkingProvider()
{
	delete m_networkItemModel;
}

const NetworkingProvider::NetworkState Kovan::NetworkingProvider::networkState() const
{
	return m_state;
}

void Kovan::NetworkingProvider::setNetworkState(const NetworkingProvider::NetworkState& state)
{
	m_state = state;
	emit networkStateChanged(state);
}

const float Kovan::NetworkingProvider::networkStrength() const
{
	return 50.0;
}

const float Kovan::NetworkingProvider::networkStrengthMin() const
{
	return 0.0;
}

const float Kovan::NetworkingProvider::networkStrengthMax() const
{
	return 100.0;
}

QAbstractItemModel *Kovan::NetworkingProvider::networkItemModel() const
{
	return m_networkItemModel;
}

FilesystemProvider *Kovan::Device::filesystemProvider() const
{
	return m_filesystemProvider;
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