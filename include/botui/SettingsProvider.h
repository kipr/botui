#ifndef _SETTINGSPROVIDER_H_
#define _SETTINGSPROVIDER_H_

#include <QObject>
#include <QString>
#include <QVariant>

class SettingsProvider : public QObject {
    Q_OBJECT
  public:
    SettingsProvider(QObject *parent = 0);
    virtual ~SettingsProvider();

    virtual void setValue(const QString &key, const QVariant &value) = 0;
    virtual QVariant value(const QString &key,
                           const QVariant &_default = QVariant()) const = 0;

  public slots:
    virtual void sync() = 0;
};

#endif
