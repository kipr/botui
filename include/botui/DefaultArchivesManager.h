#ifndef _DEFAULTARCHIVESMANAGER_H_
#define _DEFAULTARCHIVESMANAGER_H_

#include "ArchivesManager.h"

class QFileSystemWatcher;

class DefaultArchivesManager : public ArchivesManager
{
Q_OBJECT
public:
	DefaultArchivesManager(const QString &archivesPath, const QString &binariesPath, QObject *parent);
	
	virtual QStringList archives() const;
	virtual kiss::KarPtr archive(const QString &name) const;
	virtual QString binaryPath(const QString &name) const;
	virtual bool hasBinary(const QString &name) const;
	virtual bool set(const QString &name, const kiss::KarPtr &archive);
	virtual bool remove(const QString &name);
	QString archivePath(const QString &name) const;
	
private:	
	QString m_archivesPath;
	QString m_binariesPath;
	
	QFileSystemWatcher *m_watcher;
};

#endif
