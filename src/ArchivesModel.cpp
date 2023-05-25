#include "ArchivesModel.h"
#include "Config.h"
#include "Device.h"
#include "SystemPrefix.h"

#include <QDebug>
#include <QFileSystemWatcher>
#include <QStandardItem>

class ArchiveItem : public QStandardItem {
  public:
    ArchiveItem(const QString &name)
        : QStandardItem(QIcon(":/icons/bricks.png"), name), m_name(name) {}

    const QString &name() const { return m_name; }

    template <typename T> static ArchiveItem *cast(T *t) {
        return dynamic_cast<ArchiveItem *>(t);
    }

  private:
    QString m_name;
};

ArchivesModel::ArchivesModel(Device *device, QObject *parent)
    : QStandardItemModel(parent), m_device(device) {

    QFileSystemWatcher *watcher = new QFileSystemWatcher(this);

    watcher->addPath(botui::pathToKISS);

    connect(watcher, SIGNAL(directoryChanged(QString)), SLOT(refresh()));
    refresh();
}

ArchivesModel::~ArchivesModel() {}

QString ArchivesModel::name(const QModelIndex &index) const {
    ArchiveItem *const archiveItem = ArchiveItem::cast(itemFromIndex(index));
    return archiveItem ? archiveItem->name() : QString();
}

void ArchivesModel::archiveChanged(const QString &name) {
    int i = 0;
    for (; i < rowCount(); ++i) {
        ArchiveItem *archiveItem = ArchiveItem::cast(item(i));
        if (!archiveItem) continue;
        if (archiveItem->name() == name) break;
    }
    if (i < rowCount())
        insertRow(0, takeRow(i));
    else
        insertRow(0, new ArchiveItem(name));
}

void ArchivesModel::archiveRemoved(const QString &name) {
    for (int i = 0; i < rowCount(); ++i) {
        ArchiveItem *archiveItem = ArchiveItem::cast(item(i));
        if (!archiveItem) continue;
        if (archiveItem->name() == name) qDeleteAll(takeRow(i));
    }
}

void ArchivesModel::refresh() {
    clear();
    const QDir kissDir(botui::pathToKISS);

    foreach (const QString &userName,
             kissDir.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Dirs))
    {
        const QDir userDir(botui::pathToKISS + userName);
        foreach (const QString &projectName,
                 userDir.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Dirs))
        {
            const QString userAndProject = userName + "/" + projectName + "/";
            appendRow(new ArchiveItem(userAndProject));
        }
    }
}
