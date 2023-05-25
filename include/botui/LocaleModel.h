#ifndef _LOCALEMODEL_H_
#define _LOCALEMODEL_H_

#include <QLocale>
#include <QStandardItemModel>

class LocaleModel : public QStandardItemModel {
    Q_OBJECT
  public:
    LocaleModel(QObject *const parent = 0);
    ~LocaleModel();

    QLocale locale(const QModelIndex &index) const;
};

#endif