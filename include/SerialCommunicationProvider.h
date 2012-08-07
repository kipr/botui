#ifndef _SERIALCOMMUNICATIONPROVIDER_H_
#define _SERIALCOMMUNICATIONPROVIDER_H_

#include "CommunicationProvider.h"
#include <kiss-compiler/Compilation.h>

class FilesystemProvider;
class SerialIODevice;

class SerialCommunicationProvider : public CommunicationProvider
{
Q_OBJECT
public:
	SerialCommunicationProvider(Device *device, SerialIODevice *serial);
	~SerialCommunicationProvider();
	
private slots:
	void readyRead();
	
private:
	virtual const bool run(const QString& name);
	virtual CompilationPtr compile(const QString& name);
	virtual const bool download(const QString& name, TinyArchive *archive);
	void processPayload(const QByteArray& payload);
	void handleCommand(const QString& command, QDataStream& stream, QDataStream& out);
	
	Device *m_device;
	quint32 m_payload;
	SerialIODevice *m_serial;
};

#endif
