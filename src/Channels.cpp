#include "Channels.h"
#include "ConfigurationPaths.h"

#include <QFile>
#include <QDebug>

void Channels::clear()
{
	m_channels.clear();
	emit cleared();
}

Channels::index_t Channels::addChannel(const Channel &channel)
{
	if(m_channels.size() > maxChannels()) return invalidIndex();
	m_channels.append(channel);
	emit channelAdded(channel);
	return m_channels.size() - 1;
}

void Channels::removeChannel(const index_t &index)
{
	if(m_channels.size() <= index) {
		qWarning() << "Channels::removeChannel: Tried to delete non-existant channel index" << index;
		return;
	}
	
	m_channels.removeAt(index);
	emit channelRemoved(index);
}

ChannelsPtr Channels::load(const QString &name)
{
	const QString path = ConfigurationPaths::ref().channelsPath() + name;
	
	QFile file(path);
	if(!file.open(QIODevice::ReadOnly)) {
		qWarning() << "Channels::load: Couldn't open" << path << "for reading.";
		return ChannelsPtr();
	}
	
	Channels *channels = new Channels();
	
	QDataStream stream(&file);
	stream >> *channels;
	
	file.close();
	
	return ChannelsPtr(channels);
}

bool Channels::save(const QString &name) const
{
	const QString path = ConfigurationPaths::ref().channelsPath() + name;
	
	QFile file(path);
	if(!file.open(QIODevice::WriteOnly)) {
		return false;
	}
	
	QDataStream stream(&file);
	stream << *this;
	
	file.close();
	
	return true;
}

quint8 Channels::maxChannels()
{
	return 10;
}

quint8 Channels::invalidIndex()
{
	return 0xFF;
}

const ChannelList &Channels::channels() const
{
	return m_channels;
}

QDataStream &operator <<(QDataStream &stream, const Channels &channels)
{
	stream << (quint8)channels.channels().size();
	foreach(const Channel &channel, channels.channels()) {
		stream << channel;
	}
	return stream;
}

QDataStream &operator >>(QDataStream &stream, Channels &channels)
{
	channels.clear();
	
	quint8 size = 0;
	stream >> size;
	for(quint8 i = 0; i < size; ++i) {
		Channel c;
		stream >> c;
		channels.addChannel(c);
	}
	return stream;
}