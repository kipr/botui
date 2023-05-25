#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <QObject>
#include <QString>

class BatteryLevelProvider;
class NetworkingProvider;
class CommunicationProvider;
class ArchivesManager;
class CompileProvider;
class SettingsProvider;
class ButtonProvider;

class QAbstractItemModel;

typedef QList<CommunicationProvider *> CommunicationProviderList;

class Device : public QObject {
    Q_OBJECT
  public:
    virtual QString name() const = 0;
    virtual QString version() const = 0;
    virtual QString id() const = 0;
    virtual QString serial() const = 0;
    virtual bool isTouchscreen() const = 0;

    virtual CompileProvider *compileProvider() const = 0;
    virtual BatteryLevelProvider *batteryLevelProvider() const = 0;
    virtual SettingsProvider *settingsProvider() const = 0;
    virtual ButtonProvider *buttonProvider() const = 0;
};

#endif
