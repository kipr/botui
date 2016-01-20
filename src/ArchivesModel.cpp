#include "ArchivesModel.h"
#include "Device.h"
#include "SystemPrefix.h"

#include <QStandardItem>
#include <QFileSystemWatcher>
#include <QDebug>

class ArchiveItem : public QStandardItem
{
public:
	ArchiveItem(const QString &name)
		: QStandardItem(QIcon(":/icons/bricks.png"), name),
		m_name(name)
	{
	}
	
	const QString &name() const
	{
		return m_name;
	}
	
	template<typename T>
	static ArchiveItem *cast(T *t)
	{
		return dynamic_cast<ArchiveItem *>(t);
	}
private:
	QString m_name;
};

ArchivesModel::ArchivesModel(Device *device, QObject *parent)
	: QStandardItemModel(parent),
	m_device(device)
{
	
  QFileSystemWatcher *watcher = new QFileSystemWatcher(this);
  // TODO: hardcoded system path
  watcher->addPath("/home/root/Documents/KISS/bin/");
  //watcher->addPath(SystemPrefix::ref().rootManager()->archivesPath());
	
  connect(watcher, SIGNAL(directoryChanged(QString)), SLOT(refresh()));
	refresh();
}

ArchivesModel::~ArchivesModel()
{
}

QString ArchivesModel::name(const QModelIndex &index) const
{
  ArchiveItem *const archiveItem = ArchiveItem::cast(itemFromIndex(index));
  return archiveItem ? archiveItem->name() : QString();
}

void ArchivesModel::archiveChanged(const QString &name)
{
  int i = 0;
  for(; i < rowCount(); ++i) {
    ArchiveItem *archiveItem = ArchiveItem::cast(item(i));
    if(!archiveItem) continue;
    if(archiveItem->name() == name) break;
  }
  if(i < rowCount()) insertRow(0, takeRow(i));
  else insertRow(0, new ArchiveItem(name));
}

void ArchivesModel::archiveRemoved(const QString &name)
{
  for(int i = 0; i < rowCount(); ++i) {
    ArchiveItem *archiveItem = ArchiveItem::cast(item(i));
    if(!archiveItem) continue;
    if(archiveItem->name() == name) qDeleteAll(takeRow(i));
  }
}

void ArchivesModel::refresh()
{
	clear();
  // TODO: hardcoded system path
  const QDir binDir("/home/root/Documents/KISS/bin/");
  foreach(const QString &name, binDir.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Dirs)) {
    qDebug() << "Found " << name;
    if(!binDir.exists(name + "/botball_user_program")) continue;
    appendRow(new ArchiveItem(name));
  }
	/*foreach(const QString &name, SystemPrefix::ref().rootManager()->archives()
      .entryList(QDir::NoDot | QDir::NoDotDot | QDir::Files)) {
		appendRow(new ArchiveItem(name));
	}*/
}
