#ifndef _KISSCOMPILEPROVIDER_H_
#define _KISSCOMPILEPROVIDER_H_

#include <QString>
#include <QStringList>
#include <QMap>

#include "CompileProvider.h"

class Device;

class KissCompileProvider : public CompileProvider
{
public:
	KissCompileProvider(Device *device, QObject *parent = 0);
	
	Compiler::OutputList compile(const QString& name, const Kiss::KarPtr& archive);
	QString executableFor(const QString& name) const;
	
private:
	QString tempPath() const;
	QString cachePath(const QString& name) const;
	
	void sync();
	
	Device *m_device;
	QMap<QString, QString> m_executables;
};

#endif
