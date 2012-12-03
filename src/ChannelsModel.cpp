#include "ChannelsModel.h"

#include <QStandardItem>

class ChannelItem : public QStandardItem
{
public:
	ChannelItem(const Channel &channel, const QString &name)
		: QStandardItem(name + "(" + channel.modeString() + ")"),
		m_channel(channel)
	{
		setSizeHint(QSize(0, 20));
	}
	
	const Channel &channel() const
	{
		return m_channel;
	}
	
	template<typename T>
	static ChannelItem *channelitem_cast(T *t)
	{
		return dynamic_cast<ChannelItem *>(t);
	}
	
private:
	Channel m_channel;
};

void ChannelsModel::setChannels(const ChannelsPtr &channels)
{
	if(!m_channels.isNull()) m_channels->disconnect(this);
	m_channels = channels;
	clear();
	if(m_channels.isNull()) return;
	// TODO: Fill in appropriate connections
	// connect(m_channels.data(), SIGNAL())
}

const ChannelsPtr &ChannelsModel::channels() const
{
	return m_channels;
}

void ChannelsModel::addChannel(const Channel &channel)
{
	const Channels::index_t index = m_channels->channels().size() - 1;
	appendRow(new ChannelItem(channel, tr("Channel %1").arg(index)));
}

void ChannelsModel::removeChannel(const Channels::index_t &index)
{
	qDeleteAll(takeRow(index));
}