#include "LocaleModel.h"
#include <QStandardItem>
#include <QFileInfo>
#include <QSettings>
#include <QDir>

class LocaleItem : public QStandardItem
{
public:
  LocaleItem(const QLocale &locale)
    : _locale(locale)
  {
    setText(QLocale::languageToString(locale.language()));
  }
  
  const QLocale &locale() const
  {
    return _locale;
  }
  
  template<typename T>
  static LocaleItem *cast(T *const t)
  {
    return dynamic_cast<LocaleItem *>(t);
  }
  
  template<typename T>
  static const LocaleItem *cast(const T *const t)
  {
    return dynamic_cast<const LocaleItem *>(t);
  }
  
private:
  QLocale _locale;
};

LocaleModel::LocaleModel(QObject *const parent)
  : QStandardItemModel(parent)
{
  const QDir localeDir("/etc/botui/locale");
  Q_FOREACH(const QFileInfo &localeFile, localeDir.entryInfoList(QDir::NoDot | QDir::NoDotDot | QDir::Files)) {
    const QString name = localeFile.baseName();
    const int underline = name.indexOf("_");
    if(!underline) continue;
    appendRow(new LocaleItem(QLocale(name.mid(underline + 1))));
  }
}

LocaleModel::~LocaleModel()
{
}

QLocale LocaleModel::locale(const QModelIndex &index) const
{
  LocaleItem *const item = LocaleItem::cast(itemFromIndex(index));
  return item ? item->locale() : QLocale();
}