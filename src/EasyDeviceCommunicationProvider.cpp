#include "EasyDeviceCommunicationProvider.h"
#include "Device.h"
#include "FilesystemProvider.h"
#include "CompilingWidget.h"

#include <easydevice/server.hpp>
#include <easydevice/discovery_client.hpp>
#include <easydevice/device_info.hpp>

#include <pcompiler/output.hpp>

#include "RootController.h"
#include "ProgramWidget.h"
#include "CompileProvider.h"
#include "PairWidget.h"
#include "ProgramArguments.h"
#include "Program.h"

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
	QString executable = device()->compileProvider()->executableFor(name);
	if(executable.isEmpty()) return false;
	
	RootController *controller = &RootController::ref();
	
	if(controller->containsWidget<ProgramWidget>()) controller->dismissUntil<ProgramWidget>();
	else {
		ProgramWidget *programWidget = new ProgramWidget(Program::instance(), device());
		QMetaObject::invokeMethod(controller, "presentWidget", Qt::QueuedConnection,
			Q_ARG(QWidget *, programWidget));
	}
	FilesystemProvider *filesystem = device()->filesystemProvider();
	QStringList args;
	if(filesystem) {
		args = ProgramArguments::arguments(filesystem->program(name));
	}
	
	return Program::instance()->start(executable, args);
}

Compiler::OutputList EasyDeviceCommunicationProvider::compile(const QString& name)
{
	FilesystemProvider *filesystem = device()->filesystemProvider();
	if(!filesystem) return Compiler::OutputList() << Compiler::Output(name, 1,
		QByteArray(), "error: Device does not have a filesystem provider.");
	
	Kiss::KarPtr archive = filesystem->program(name);
	if(!archive) return Compiler::OutputList() << Compiler::Output(name, 1,
		QByteArray(), "error: Failed to load KISS Archive.");
	
	return device()->compileProvider()->compile(name, archive);
}

const bool EasyDeviceCommunicationProvider::download(const QString& name, const Kiss::KarPtr& archive)
{
	FilesystemProvider *filesystem = device()->filesystemProvider();
	qDebug() << "Calling setProgram";
	return filesystem && filesystem->setProgram(name, archive);
}

bool EasyDeviceCommunicationProvider::deleteProgram(const QString& name)
{
	// TODO: NYI
	return false;
}

QStringList EasyDeviceCommunicationProvider::list() const
{
	// TODO: NYI
	return QStringList();
}

QString EasyDeviceCommunicationProvider::interaction(const QString& command)
{
	// TODO: NYI
	return QString();
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

