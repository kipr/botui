#include "FileUtils.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>

bool FileUtils::copy(const QString &path, const QString &newPath) {
    QFileInfo input(path);
    if (!input.exists()) return false;

    if (input.isDir()) {
        if (!QDir(newPath).mkpath(input.fileName())) {
            qWarning() << "QDir(newPath).mkpath(input.fileName())"
                       << "failed. (newPath =" << newPath << ")";
            return false;
        }
        QFileInfoList entries =
            QDir(input.absoluteFilePath())
                .entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
        qDebug() << QDir(input.absoluteFilePath())
                        .entryList(QDir::NoDot | QDir::NoDotDot);
        foreach (const QFileInfo &entry, entries) {
            if (!copy(entry.absoluteFilePath(),
                      newPath + "/" + input.fileName())) {
                return false;
            }
        }
        return true;
    }
    else if (input.isFile()) {
        return QFile::copy(path, newPath + "/" + input.fileName());
    }

    return false;
}

bool FileUtils::rm(const QString &path) {
    QFileInfo input(path);
    if (input.isDir()) {
        qDebug() << "In directory" << input.absoluteFilePath();
        QFileInfoList entries =
            QDir(input.absoluteFilePath())
                .entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
        qDebug() << QDir(input.absoluteFilePath())
                        .entryList(QDir::NoDotAndDotDot | QDir::Files |
                                   QDir::Dirs);
        foreach (const QFileInfo &entry, entries) {
            qDebug() << "Recursively removing" << entry.absoluteFilePath();
            if (!rm(entry.absoluteFilePath())) {
                return false;
            }
        }
        qDebug() << "Removing" << path;
        return QDir().rmdir(input.absoluteFilePath());
    }
    else if (input.isFile()) {
        qDebug() << "Removing" << path;
        return QFile::remove(path);
    }

    return false;
}