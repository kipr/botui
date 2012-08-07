#include "SerialCommunicationProvider.h"
#include "FilesystemProvider.h"
#include "CompileProvider.h"
#include "SerialIODevice.h"
#include "RootController.h"
#include "Device.h"
#include "ProgramWidget.h"
#include "Program.h"
#include <kiss-compiler/QTinyArchiveStream.h>

#include <QDebug>

#define AVAILABLE_KEY "available?"
#define COMPILE_KEY "compile"
#define DOWNLOAD_KEY "download"
#define RUN_KEY "run"
#define LOCKED_KEY "locked"
#define AUTHENTICATE_KEY "authenticate"

SerialCommunicationProvider::SerialCommunicationProvider(Device *device, const QString& path)
	: CommunicationProvider(device), m_serial(new SerialIODevice(path))
{
	connect(m_serial, SIGNAL(readyRead()), SLOT(readyRead()));
	m_serial->open(QIODevice::ReadWrite);
}

SerialCommunicationProvider::~SerialCommunicationProvider()
{
	m_serial->close();
	delete m_serial;
}

const bool SerialCommunicationProvider::run(const QString& name)
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
	return Program::instance()->start(executable);
}

CompilationPtr SerialCommunicationProvider::compile(const QString& name)
{
	FilesystemProvider *filesystem = device()->filesystemProvider();
	if(!filesystem) return CompilationPtr();

	TinyArchive *archive = filesystem->program(name);
	if(!archive) return CompilationPtr();

	return device()->compileProvider()->compile(name, archive);
}

const bool SerialCommunicationProvider::download(const QString& name, TinyArchive *archive)
{
	FilesystemProvider *filesystem = device()->filesystemProvider();
	qDebug() << "Calling setProgram";
	return filesystem && filesystem->setProgram(name, archive);
}

void SerialCommunicationProvider::handleCommand(const QString& command, QDataStream& stream, QDataStream& out)
{
	qDebug() << "Processing command" << command << "...";
	
	out << command;
	if(command == RUN_KEY) {
		QString name;
		stream >> name;
		qDebug() << "Running" << name << "...";
		bool success = run(name);
		out << success;
	} else if(command == COMPILE_KEY) {
		QString name;
		stream >> name;
		qDebug() << "Compiling" << name << "...";
		CompilationPtr compilation = compile(name);
		if(!compilation.get()) out << CompileResult(false);
		else out << compilation->results();
	} else if(command == DOWNLOAD_KEY) {
		QString name;
		stream >> name;
		qDebug() << "Receiving" << name << "...";
		QTinyArchiveStream reader(stream.device());
		TinyArchive *archive = reader.read();
		if(archive) download(name, archive);
		bool success = archive;
		out << success;
	} else if(command == AVAILABLE_KEY) {
		bool avail = true;
		out << avail;
	} else qCritical() << "Unknown command" << command;
}

void SerialCommunicationProvider::readyRead()
{
	for(;;) {
		if(!m_payload && m_serial->bytesAvailable() < sizeof(quint32)) return;
		if(!m_payload && m_serial->bytesAvailable() >= sizeof(quint32)) {
			QDataStream stream(m_serial);
			stream >> m_payload;
			qDebug() << "Awaiting a payload of size" << m_payload;
		}
		if(!m_payload) return;
	
		qDebug() << m_serial->bytesAvailable() << "vs payload" << m_payload;
	
		if(m_serial->bytesAvailable() >= m_payload) {
			QByteArray payload = m_serial->read(m_payload);
			processPayload(payload);
			m_payload = 0;
		} else break;
	}
}

void SerialCommunicationProvider::processPayload(const QByteArray& payload)
{
	QDataStream stream(payload);
	QString command;
	stream >> command;
	QByteArray block;
	QDataStream outStream(&block, QIODevice::WriteOnly);
	outStream << (quint32)0;
	handleCommand(command, stream, outStream);
	outStream.device()->seek(0);
	outStream << (quint32)(block.size() - sizeof(quint32));
	m_serial->write(block);
	qDebug() << "Writing data back with size" << block.size();
}
