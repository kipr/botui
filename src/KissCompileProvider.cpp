#include "KissCompileProvider.h"

#include <kiss-compiler/QTinyArchive.h>
#include <kiss-compiler/Temporary.h>
#include <kiss-compiler/ArchiveWriter.h>
#include <kiss-compiler/CompilerManager.h>

#include "RootController.h"
#include "CompilingWidget.h"

#include <QDebug>

KissCompileProvider::KissCompileProvider(QObject *parent)
	: CompileProvider(parent)
{
	
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
	
	/* if(success) m_compileResults[name] = compilation->compileResults();
	else m_compileResults.remove(name); */
	
	return compilation;
}