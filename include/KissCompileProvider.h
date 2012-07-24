#ifndef _KISSCOMPILEPROVIDER_H_
#define _KISSCOMPILEPROVIDER_H_

#include <QString>
#include <QStringList>
#include <QMap>

#include "CompileProvider.h"

class TinyArchive;

class KissCompileProvider : public CompileProvider
{
public:
	KissCompileProvider(QObject *parent = 0);
	
	CompilationPtr compile(const QString& name, TinyArchive *archive);
	QString executableFor(const QString& name) const;
	
private:
	void sync();
	
	QMap<QString, QStringList> m_executables;
};

#endif
