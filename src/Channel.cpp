#include "Channel.h"

Channel::Channel()
	: m_mode(Channel::Unknown)
{
}

Channel::Channel(const Channel::Mode &mode, const QVariant &data)
	: m_mode(mode),
	m_data(data)
{
}

void Channel::setMode(const Channel::Mode &mode)
{
	m_mode = mode;
}

const Channel::Mode &Channel::mode() const
{
	return m_mode;
}

QString Channel::modeString() const
{
	switch(m_mode) {
	case Channel::Unknown: return QObject::tr("Unknown");
	case Channel::HsvBlobTracking: return QObject::tr("HSV Blob Tracking");
	case Channel::FaceDetection: return QObject::tr("Face Detection");
	}
	
	return QObject::tr("Invalid");
}

void Channel::setData(const QVariant &data)
{
	m_data = data;
}

const QVariant &Channel::data() const
{
	return m_data;
}

QDataStream &operator <<(QDataStream &stream, const Channel &channel)
{
	stream << (quint16)channel.mode();
	stream << channel.data();
	return stream;
}

QDataStream &operator >>(QDataStream &stream, Channel &channel)
{
	quint16 mode = Channel::Unknown;
	stream >> mode;
	channel.setMode((Channel::Mode)mode);
	
	QVariant data;
	stream << data;
	channel.setData(data);
	return stream;
}