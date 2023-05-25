#ifndef _KISSCOMPILEPROVIDER_H_
#define _KISSCOMPILEPROVIDER_H_

#include <QMap>
#include <QString>
#include <QStringList>

#include "CompileProvider.h"

class Device;

class KissCompileProvider : public CompileProvider {
  public:
    KissCompileProvider(Device *device, QObject *parent = 0);
    Compiler::OutputList compile(const QString &name,
                                 const kiss::KarPtr &archive,
                                 const QString &install_dir);

  private:
    QString tempPath() const;

    Device *m_device;
    QMap<QString, QString> m_executables;
};

#endif
