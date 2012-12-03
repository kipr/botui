#ifndef _CHANNELSMODEL_H_
#define _CHANNELSMODEL_H_

#include "Channels.h"

#include <QObject>
#include <QStandardItemModel>

class ChannelsModel : public QStandardItemModel
{
Q_OBJECT
public:
	ChannelsModel(QObject *parent = 0);

	Q_PROPERTY(ChannelsPtr channels READ channels WRITE setChannels)
	void setChannels(const ChannelsPtr &channels);
	const ChannelsPtr &channels() const;
	
private slots:
	void addChannel(const Channel &channel);
	void removeChannel(const Channels::index_t &index);
	
private:
	ChannelsPtr m_channels;
};

#endif
