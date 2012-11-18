#ifndef _NETWORKMANAGERCONNECTION_H_
#define _NETWORKMANAGERCONNECTION_H_

#include <QString>
#include <QMap>
#include <QVariant>

typedef QMap<QString, QMap<QString, QVariant> > Connection;
Q_DECLARE_METATYPE(Connection)

#endif
