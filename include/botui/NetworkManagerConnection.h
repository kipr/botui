#include "Options.h"

#ifdef NETWORK_ENABLED

#ifndef _NETWORKMANAGERCONNECTION_H_
#define _NETWORKMANAGERCONNECTION_H_

#include <QMap>
#include <QString>
#include <QVariant>

typedef QMap<QString, QVariant> StringVariantMap;
typedef QMap<QString, StringVariantMap> Connection;
typedef QList<StringVariantMap> ListStringVariantMap;
// Connection = QMap<QString, QMap<QString,QVariant>>
Q_DECLARE_METATYPE(StringVariantMap);
Q_DECLARE_METATYPE(Connection);
Q_DECLARE_METATYPE(ListStringVariantMap);

#endif

#endif