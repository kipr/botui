#ifndef _WALLABY_SETTINGS_PROVIDER_H
#define _WALLABY_SETTINGS_PROVIDER_H

#include "SettingsProvider.h"

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QFile>

namespace Wallaby
{
  class SettingsProvider : public ::SettingsProvider
  {
  public:
    SettingsProvider(QObject *parent = 0);
    ~SettingsProvider();
    virtual void setValue(const QString& key, const QVariant& value);
    virtual QVariant value(const QString& key, const QVariant& _default = QVariant()) const;
    virtual void sync();
  private:
    QMap<QString, QVariant> m_settings;
    QFile m_settingsFile;
  };
}

#endif