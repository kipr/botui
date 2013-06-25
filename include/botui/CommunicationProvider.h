#ifndef _COMMUNICATIONPROVIDER_H_
#define _COMMUNICATIONPROVIDER_H_

#include <QObject>
#include <kar.hpp>

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
	void programReceived(const Kiss::KarPtr& archive);
	
private:
	Device *m_device;
};

#endif
