#ifndef _EASYDEVICECOMMUNICATIONPROVIDER_H_
#define _EASYDEVICECOMMUNICATIONPROVIDER_H_

#include "CommunicationProvider.h"
#include <easydevice/ServerDelegate.h>

namespace EasyDevice
{
	class DiscoveryClient;
	class Server;
}

class FilesystemProvider;

class EasyDeviceCommunicationProvider : public CommunicationProvider, public EasyDevice::ServerDelegate
{
Q_OBJECT
public:
	EasyDeviceCommunicationProvider(Device *device);
	~EasyDeviceCommunicationProvider();
	
private:
	virtual const bool run(const QString& name);
	virtual CompilationPtr compile(const QString& name);
	virtual const bool download(const QString& name, TinyArchive *archive);
	
	EasyDevice::Server *m_server;
	EasyDevice::DiscoveryClient *m_discovery;
};

#endif
