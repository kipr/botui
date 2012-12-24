#include "ChannelConfigurationsModel.h"

#include <QDir>
#include <QFileIconProvider>

#include <kovan/camera.hpp>

class FileIconProvider : public QFileIconProvider
{
public:
	FileIconProvider()
		: m_icon(QIcon(":/icons/photos.png"))
	{
		
	}
	
	virtual QIcon icon(const QFileInfo &info) const
	{
		return m_icon;
	}
	
private:
	QIcon m_icon;
};

ChannelConfigurationsModel::ChannelConfigurationsModel(QObject *parent)
	: QFileSystemModel(parent),
	m_iconProvider(new FileIconProvider)
{
	setIconProvider(m_iconProvider);
	const QString configPath = QString::fromStdString(Camera::ConfigPath::path());
	QDir().mkpath(configPath);
	setNameFilters(QStringList() << ("*." + QString::fromStdString(Camera::ConfigPath::extension())));
	setNameFilterDisables(false);
	setRootPath(configPath);
}

ChannelConfigurationsModel::~ChannelConfigurationsModel()
{
	delete m_iconProvider;
}

QModelIndex ChannelConfigurationsModel::defaultConfiguration() const
{
	return index(QString::fromStdString(Camera::ConfigPath::defaultConfigPath()));
}