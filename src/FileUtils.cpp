#include "FileUtils.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

bool FileUtils::copy(const QString &path, const QString &newPath)
{
	QFileInfo input(path);
	if (!input.exists())
		return false;

	if (input.isDir())
	{
		if (!QDir(newPath).mkpath(input.fileName()))
		{
			qWarning() << "QDir(newPath).mkpath(input.fileName())"
								 << "failed. (newPath =" << newPath << ")";
			return false;
		}
		QFileInfoList entries = QDir(input.absoluteFilePath()).entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
		qDebug() << QDir(input.absoluteFilePath()).entryList(QDir::NoDot | QDir::NoDotDot);
		foreach (const QFileInfo &entry, entries)
		{
			if (!copy(entry.absoluteFilePath(), newPath + "/" + input.fileName()))
			{
				return false;
			}
		}
		return true;
	}
	else if (input.isFile())
	{
		return QFile::copy(path, newPath + "/" + input.fileName());
	}

	return false;
}


bool FileUtils::rm(const QString &path)
{
	QDir dir(path);

	if (dir.removeRecursively())
	{
		qDebug() << "Directory removed successfully.";
	}
	else
	{
		qDebug() << "Failed to remove directory.";
	}
}
