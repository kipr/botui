#include "KissCompileProvider.h"

#include <TinyArchive.h>
#include <kiss-compiler/QTinyArchive.h>
#include <kiss-compiler/Temporary.h>
#include <kiss-compiler/ArchiveWriter.h>
#include <kiss-compiler/CompilerManager.h>

#include "RootController.h"
#include "CompilingWidget.h"

#include <QSettings>
#include <QDebug>

#define KISS_COMPILE_GROUP "kiss_compile"
#define EXECUTABLES_KEY "executables"

KissCompileProvider::KissCompileProvider(QObject *parent)
	: CompileProvider(parent)
{
	QSettings settings;
	settings.beginGroup(KISS_COMPILE_GROUP);
	QMap<QString, QVariant> map = settings.value(EXECUTABLES_KEY).toMap();
	foreach(const QString& key, map.keys()) m_executables[key] = map[key].toStringList();
	settings.endGroup();
}

CompilationPtr KissCompileProvider::compile(const QString& name, TinyArchive *archive)
{
	if(!archive) return CompilationPtr();
	
	ArchiveWriter writer(archive, Temporary::subdir(name));
	QMap<QString, QString> settings;
	QByteArray rawSettings = QTinyNode::data(archive->lookup("settings:"));
	QDataStream stream(rawSettings);
	stream >> settings;
	
	CompilationPtr compilation(new Compilation(CompilerManager::ref().compilers(), name, writer.files(), settings, "kovan"));
	bool success = compilation->start();
	
	qDebug() << "Results:" << compilation->compileResults();
	qDebug() << (success ? "Compile Succeeded" : "Compile Failed");
	
	if(success) m_executables[name] = compilation->compileResults();
	else m_executables.remove(name);
	
	sync();
	
	return compilation;
}

QString KissCompileProvider::executableFor(const QString& name) const
{
	return m_executables.value(name, QStringList() << "")[0];
}

void KissCompileProvider::sync()
{
	QSettings settings;
	settings.beginGroup(KISS_COMPILE_GROUP);
	QMap<QString, QVariant> map;
	foreach(const QString& key, m_executables.keys()) map[key] = QVariant(m_executables[key]);
	settings.setValue(EXECUTABLES_KEY, map);
	settings.endGroup();
}