#ifndef _ARCHIVESMANAGER_H_
#define _ARCHIVESMANAGER_H_

#include <QObject>
#include <kar.hpp>

class ArchivesManager : public QObject
{
Q_OBJECT
public:
	ArchivesManager(QObject *parent = 0);
	
	virtual QStringList archives() const = 0;
	virtual Kiss::KarPtr archive(const QString &name) const = 0;
	virtual QString binaryPath(const QString &name) const = 0;
	virtual bool hasBinary(const QString &name) const = 0;
	virtual bool set(const QString &name, const Kiss::KarPtr &archive) = 0;
	virtual bool remove(const QString &name) = 0;
	
signals:
	void archiveChanged(const QString &name);
	void archiveRemoved(const QString &name);
	void refresh();
};

#endif
