#include "EasyDeviceCommunicationProvider.h"
#include "Device.h"
#include "FilesystemProvider.h"
#include "CompilingWidget.h"

#include <easydevice/Server.h>
#include <easydevice/DiscoveryClient.h>
#include <easydevice/DeviceInfo.h>

#include <kiss-compiler/ArchiveWriter.h>
#include <kiss-compiler/Temporary.h>
#include <kiss-compiler/CompilerManager.h>

#include "RootController.h"
#include "ProgramWidget.h"
#include "CompileProvider.h"
#include "PairWidget.h"

#include <QMetaObject>
#include <QCryptographicHash>
#include <QDebug>

EasyDeviceCommunicationProvider::EasyDeviceCommunicationProvider(Device *device)
	: CommunicationProvider(device),
	m_server(new EasyDevice::Server(this, this)),
	m_discovery(new EasyDevice::DiscoveryClient(this)),
	m_generator(EasyDevice::PasswordGenerator::Numbers | EasyDevice::PasswordGenerator::Letters),
	m_authing(false)
{
	bool success = true;
	
	EasyDevice::DeviceInfo deviceInfo;
	deviceInfo.setDeviceType("kovan");
	deviceInfo.setDisplayName("Kovan 1234");
	deviceInfo.setSerialNumber("N/A");
	
	if(!m_discovery->setup()) {
		qDebug() << "Failed to setup listener";
		success &= false;
	}
	m_discovery->setDeviceInfo(deviceInfo);
	if(!m_server->listen(QHostAddress::Any, 8075)) {
		qDebug() << "Failed to listen";
		success &= false;
	}
}

EasyDeviceCommunicationProvider::~EasyDeviceCommunicationProvider()
{
	delete m_server;
	delete m_discovery;
}

void EasyDeviceCommunicationProvider::canceled()
{
	RootController::ref().setDismissable(true);
	m_hash = QByteArray();
	m_authing = false;
	QMetaObject::invokeMethod(&RootController::ref(), "dismissWidget", Qt::QueuedConnection);
}

const bool EasyDeviceCommunicationProvider::run(const QString& name)
{
	ProgramWidget *programWidget = new ProgramWidget(name, device());
	// Must invoke this method on the gui thread
	QMetaObject::invokeMethod(&RootController::ref(), "presentWidget", Qt::QueuedConnection, Q_ARG(QWidget *, programWidget));
	return programWidget->start();
}

CompilationPtr EasyDeviceCommunicationProvider::compile(const QString& name)
{
	FilesystemProvider *filesystem = device()->filesystemProvider();
	if(!filesystem) return CompilationPtr();
	
	TinyArchive *archive = filesystem->program(name);
	if(!archive) return CompilationPtr();
	
	return device()->compileProvider()->compile(name, archive);
}

const bool EasyDeviceCommunicationProvider::download(const QString& name, TinyArchive *archive)
{
	FilesystemProvider *filesystem = device()->filesystemProvider();
	qDebug() << "Calling setProgram";
	return filesystem && filesystem->setProgram(name, archive);
}

const bool EasyDeviceCommunicationProvider::isAuthenticated(const QHostAddress& address)
{
	return m_currentAddress == address;
}

const bool EasyDeviceCommunicationProvider::authenticationRequest(const QHostAddress& address)
{
	if(m_authing) return false;
	m_authing = true;
	
	QString password = m_generator.password();
	PairWidget *pairWidget = new PairWidget(device());
	connect(pairWidget, SIGNAL(cancel()), SLOT(canceled()));
	RootController::ref().setDismissable(false);
	// Must invoke this method on the gui thread
	QMetaObject::invokeMethod(&RootController::ref(), "presentWidget", Qt::QueuedConnection, Q_ARG(QWidget *, pairWidget));
	pairWidget->setPassword(password);
	QCryptographicHash hasher(QCryptographicHash::Sha1);
	hasher.addData(password.toUtf8());
	m_hash = hasher.result();
	return true;
}

const EasyDevice::ServerDelegate::AuthenticateReturn EasyDeviceCommunicationProvider::authenticate(const QHostAddress& address, const QByteArray& hash)
{
	if(m_hash.isNull()) return EasyDevice::ServerDelegate::AuthWillNotAccept;
	if(hash.isNull() || m_hash != hash) return EasyDevice::ServerDelegate::AuthTryAgain;
	canceled();
	m_currentAddress = address;
	qDebug() << "Paired with " << m_currentAddress.toString();
	return EasyDevice::ServerDelegate::AuthSuccess;
}

