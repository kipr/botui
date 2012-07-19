#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <QObject>
#include <QString>

class BatteryLevelProvider;
class NetworkingProvider;
class CommunicationProvider;
class FilesystemProvider;
class CompileProvider;

class QAbstractItemModel;

typedef QList<CommunicationProvider *> CommunicationProviderList;

class Device : public QObject
{
Q_OBJECT
public:
	virtual QString name() const = 0;
	virtual QString version() const = 0;
	
	virtual FilesystemProvider *filesystemProvider() const = 0;
	virtual CompileProvider *compileProvider() const = 0;
	virtual CommunicationProviderList communicationProviders() const = 0;
	virtual NetworkingProvider *networkingProvider() const = 0;
	virtual BatteryLevelProvider *batteryLevelProvider() const = 0;
};

#endif
