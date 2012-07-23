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

#include <QDebug>

EasyDeviceCommunicationProvider::EasyDeviceCommunicationProvider(Device *device)
	: CommunicationProvider(device),
	m_server(new EasyDevice::Server(this, this)),
	m_discovery(new EasyDevice::DiscoveryClient(this))
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

const bool EasyDeviceCommunicationProvider::run(const QString& name)
{
	ProgramWidget *programWidget = new ProgramWidget(name, device());
	RootController::ref().presentWidget(programWidget);
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