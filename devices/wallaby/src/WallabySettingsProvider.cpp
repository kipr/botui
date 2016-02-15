#include "WallabySettingsProvider.h"

#include <QDir>

Wallaby::SettingsProvider::SettingsProvider(QObject *parent)
  : ::SettingsProvider(parent),
  m_settingsFile(QDir::homePath() + "/botui_settings", this)
{
  if(!m_settingsFile.open(QIODevice::ReadOnly)) return;
  QDataStream stream(&m_settingsFile);
  stream >> m_settings;
  m_settingsFile.close();
}

Wallaby::SettingsProvider::~SettingsProvider()
{
  sync();
}

void Wallaby::SettingsProvider::setValue(const QString& key, const QVariant& value)
{
  m_settings[key] = value;
}

QVariant Wallaby::SettingsProvider::value(const QString& key, const QVariant& _default) const
{
  return m_settings.value(key, _default);
}

void Wallaby::SettingsProvider::sync()
{
  if(!m_settingsFile.open(QIODevice::WriteOnly)) return;
  QDataStream stream(&m_settingsFile);
  stream << m_settings;
  m_settingsFile.close();
  
  emit settingsChanged();
}