#ifndef _ARCHIVESMODEL_H_
#define _ARCHIVESMODEL_H_

#include <QStandardItemModel>
#include <kar/kar.hpp>

class Device;
class ArchivesModel;

class ArchivesModel : public QStandardItemModel {
    Q_OBJECT
  public:
    ArchivesModel(Device *device, QObject *parent = 0);
    ~ArchivesModel();

    QString name(const QModelIndex &index) const;

  private slots:
    void archiveChanged(const QString &name);
    void archiveRemoved(const QString &name);
    void refresh();

  private:
    Device *m_device;
};

#endif
