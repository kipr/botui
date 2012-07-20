#ifndef _PACKAGEMANAGERPROVIDER_H_
#define _PACKAGEMANAGERPROVIDER_H_

#include <QObject>

class PackageManagerProvider : public QObject
{
Q_OBJECT
public:
	PackageManagerProvider(QObject *parent = 0);
	virtual ~PackageManagerProvider();
	
	virtual unsigned int numberOfUpdatesAvailable() = 0;
	virtual void refresh() = 0;
	
	virtual QStringList installed() const = 0;
	virtual QStringList updatable() const = 0;
	virtual QStringList availableForInstall() const = 0;
	virtual QStringList all() const = 0;
	
signals:
	void refreshed();
};

#endif
