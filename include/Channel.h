#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <QList>
#include <QVariant>
#include <QString>

class Channel
{
public:
	enum Mode {
		Unknown = 0,
		HsvBlobTracking,
		FaceDetection,
	};
	
	Channel();
	Channel(const Mode &mode, const QVariant &data);
	
	void setMode(const Mode &mode);
	const Mode &mode() const;
	QString modeString() const;
	
	void setData(const QVariant &data);
	const QVariant &data() const;
	
private:
	Mode m_mode;
	QVariant m_data;
};

typedef QList<Channel> ChannelList;

QDataStream &operator <<(QDataStream &stream, const Channel &channel);
QDataStream &operator >>(QDataStream &stream, Channel &channel);

#endif
