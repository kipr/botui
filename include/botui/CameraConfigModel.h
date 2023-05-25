#ifndef _CAMERACONFIGMODEL_H_
#define _CAMERACONFIGMODEL_H_

#include <QObject>
#include <QStandardItemModel>

#include <kipr/config/config.hpp>

using namespace kipr::config;

class CameraConfigModel : public QStandardItemModel {
    Q_OBJECT
  public:
    CameraConfigModel(QObject *parent = 0);
    ~CameraConfigModel();

    const QString &channelType(const QModelIndex &index) const;

    Q_PROPERTY(Config config READ config WRITE setConfig)
    void setConfig(const Config &config);
    const Config &config() const;

  public slots:
    void addChannel(const QString &type);
    void removeChannel(const int &i);
    void swapChannels(const int &i, const int &j);

    Config channelConfig(const QModelIndex &index) const;
    void setChannelConfig(const QModelIndex &index, const Config &config);

  private:
    void updateConfig();

    Config m_config;
    QString m_blank;
};

#endif
