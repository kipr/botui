#ifndef _CHANNELS_H_
#define _CHANNELS_H_

#include "Channel.h"

#include <QObject>
#include <QDataStream>
#include <QSharedPointer>

class Channels;

typedef QSharedPointer<Channels> ChannelsPtr;

class Channels : public QObject
{
Q_OBJECT
public:
	typedef quint8 index_t;
	
	void clear();
	index_t addChannel(const Channel &channel);
	void removeChannel(const index_t &index);
	
	static ChannelsPtr load(const QString &name);
	bool save(const QString &name) const;
	
	static quint8 maxChannels();
	static quint8 invalidIndex();
	
	const ChannelList &channels() const;
	
signals:
	void cleared();
	void channelAdded(const Channel &channel);
	void channelRemoved(const index_t &index);
	
private:
	ChannelList m_channels;
};

QDataStream &operator <<(QDataStream &stream, const Channels &channels);
QDataStream &operator >>(QDataStream &stream, Channels &channels);

#endif
