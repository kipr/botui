#include "DefaultArchivesManager.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>

DefaultArchivesManager::DefaultArchivesManager(const QString &archivesPath,
	const QString &binariesPath, QObject *parent)
	: ArchivesManager(parent),
	m_archivesPath(archivesPath),
	m_binariesPath(binariesPath),
	m_watcher(new QFileSystemWatcher(this))
{
	m_watcher->addPath(m_archivesPath);
	connect(m_watcher, SIGNAL(directoryChanged(QString)),
		SIGNAL(refresh()));
}

QStringList DefaultArchivesManager::archives() const
{
	return QDir(m_archivesPath).entryList(QDir::NoDot | QDir::NoDotDot | QDir::Files);
}

Kiss::KarPtr DefaultArchivesManager::archive(const QString &name) const
{
	return Kiss::Kar::load(archivePath(name));
}

QString DefaultArchivesManager::binaryPath(const QString &name) const
{
	return QDir(m_binariesPath).filePath(name);
}

bool DefaultArchivesManager::hasBinary(const QString &name) const
{
	return QFile::exists(binaryPath(name));
}

bool DefaultArchivesManager::set(const QString &name, const Kiss::KarPtr &archive)
{
	bool ret = archive->save(archivePath(name));
	if(ret) emit archiveChanged(name);
	return ret;
}

bool DefaultArchivesManager::remove(const QString &name)
{
	bool success = true;
	success &= QFile::remove(archivePath(name));
	success &= !hasBinary(name) || QFile::remove(binaryPath(name));
	return success;
}

QString DefaultArchivesManager::archivePath(const QString &name) const
{
	return QDir(m_archivesPath).filePath(name);
}