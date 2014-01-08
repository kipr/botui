#include "Options.h"

#ifdef NETWORK_ENABLED

#ifndef _NETWORKMANAGERCONNECTION_H_
#define _NETWORKMANAGERCONNECTION_H_

#include <QString>
#include <QMap>
#include <QVariant>

typedef QMap<QString, QVariant> StringVariantMap;
typedef QMap<QString, StringVariantMap> Connection;
Q_DECLARE_METATYPE(StringVariantMap);
Q_DECLARE_METATYPE(Connection);

#endif

#endif