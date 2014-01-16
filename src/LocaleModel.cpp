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
  QDir locales("/etc/botui/locale");
  
  Q_FOREACH(const QFileInfo &localeFile, locales.entryInfoList(QDir::NoDot | QDir::NoDotDot
      | QDir::Files)) {
    const QString name = localeFile.baseName();
    const int underline = name.indexOf("_");
    if(!underline) continue;
    const QString code = name.mid(underline + 1);
    appendRow(new LocaleItem(QLocale(code)));
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