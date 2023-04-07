#ifndef _CHANNELCONFIGURATIONSMODEL_H_
#define _CHANNELCONFIGURATIONSMODEL_H_

#include <QFileSystemModel>
#include <QFileIconProvider>
class ChannelConfigurationsModel : public QFileSystemModel
{
Q_OBJECT
public:
        ChannelConfigurationsModel(QObject *parent = 0);
        ~ChannelConfigurationsModel();
        
        QModelIndex defaultConfiguration() const;
private:
        QFileIconProvider *m_iconProvider;
};

#endif
