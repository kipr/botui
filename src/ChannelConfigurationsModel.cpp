#include "ChannelConfigurationsModel.h"

#include <QDir>
#include <QFileIconProvider>

#include <kipr/camera/camera.hpp>

class FileIconProvider : public QFileIconProvider {
  public:
    FileIconProvider() : m_icon(QIcon(":/icons/photos.png")) {}

    virtual QIcon icon(const QFileInfo &info) const { return m_icon; }

  private:
    QIcon m_icon;
};

ChannelConfigurationsModel::ChannelConfigurationsModel(QObject *parent)
    : QFileSystemModel(parent), m_iconProvider(new FileIconProvider) {
    setIconProvider(m_iconProvider);
    const QString configPath =
        QString::fromStdString(kipr::camera::ConfigPath::path());
    QDir().mkpath(configPath);
    setNameFilters(QStringList()
                   << ("*." + QString::fromStdString(
                                  kipr::camera::ConfigPath::extension())));
    setNameFilterDisables(false);
    setRootPath(configPath);
}

ChannelConfigurationsModel::~ChannelConfigurationsModel() {
    delete m_iconProvider;
}

QModelIndex ChannelConfigurationsModel::defaultConfiguration() const {
    return index(
        QString::fromStdString(kipr::camera::ConfigPath::defaultConfigPath()));
}
