#ifndef _EASYDEVICECOMMUNICATIONPROVIDER_H_
#define _EASYDEVICECOMMUNICATIONPROVIDER_H_

#include "CommunicationProvider.h"
#include <easydevice/server_delegate.hpp>
#include <easydevice/password_generator.hpp>
#include <QHostAddress>

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
	
private slots:
	void canceled();
	
private:
	virtual const bool run(const QString& name);
	virtual Compiler::OutputList compile(const QString& name);
	virtual const bool download(const QString& name, const Kiss::KarPtr& archive);
	virtual bool deleteProgram(const QString& name);
	virtual QStringList list() const;
	virtual QString interaction(const QString& command);
	
	virtual const bool isAuthenticated(const QHostAddress& address);
	virtual const bool authenticationRequest(const QHostAddress& address);
	virtual const EasyDevice::ServerDelegate::AuthenticateReturn authenticate(const QHostAddress& address, const QByteArray& hash);
	
	EasyDevice::Server *m_server;
	EasyDevice::DiscoveryClient *m_discovery;
	
	QHostAddress m_currentAddress;
	QByteArray m_hash;
	EasyDevice::PasswordGenerator m_generator;
	
	bool m_authing;
};

#endif
