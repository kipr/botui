#ifndef _CHANNELCONFIGWIDGET_H_
#define _CHANNELCONFIGWIDGET_H_

#include <QWidget>

#include <kipr/config/config.hpp>

using namespace kipr::config;

#include <QModelIndex>

class ChannelConfigWidget : public QWidget {
    Q_OBJECT
  public:
    ChannelConfigWidget(const QModelIndex &index, QWidget *parent = 0);
    virtual ~ChannelConfigWidget();

    Q_PROPERTY(Config config READ config WRITE setConfig NOTIFY configChanged)
    void setConfig(const Config &config);
    const Config &config() const;

    virtual void refresh() = 0;

  signals:
    void configChanged(const QModelIndex &index, const Config &config);

  private:
    Config m_config;
    QModelIndex m_index;
};

#endif
