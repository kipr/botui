#ifndef _COMMUNICATIONPROVIDER_H_
#define _COMMUNICATIONPROVIDER_H_

#include <QObject>

class TinyArchive;
class Device;

class CommunicationProvider : public QObject
{
Q_OBJECT
public:
	CommunicationProvider(Device *device);
	virtual ~CommunicationProvider();
	
	void setDevice(Device *device);
	Device *device() const;
signals:
	void programReceived(const TinyArchive *tinyArchive);
	
private:
	Device *m_device;
};

#endif
