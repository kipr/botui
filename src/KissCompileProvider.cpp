#include "KissCompileProvider.h"

#include <pcompiler/pcompiler.hpp>

#include "RootController.h"
#include "CompilingWidget.h"
#include "Device.h"

#include <QDir>
#include <QFile>
#include <QDateTime>

#include <QSettings>
#include <QDebug>

#define KISS_COMPILE_GROUP "kiss_compile"
#define EXECUTABLES_KEY "executables"

using namespace Compiler;

KissCompileProvider::KissCompileProvider(Device *device, QObject *parent)
	: CompileProvider(parent),
	m_device(device)
{
}

Compiler::OutputList KissCompileProvider::compile(const QString& name, const Kiss::KarPtr& program)
{
	if(program.isNull()) {
		return OutputList() << Output(name, 1,
			QByteArray(), "error: KarPtr is null");
	}
	
	const QString path = tempPath();
	qDebug() << path;
	
	QMap<QString, QString> settings;
	if(!program->extract(path)) {
		return OutputList() << Output(name, 1,
			QByteArray(), "error: Failed to extract KISS Archive.");
	}
	
	QStringList extractedFiles;
	foreach(const QString& file, program->files()) {
		qDebug() << "Extracted" << path + "/" + file;
		extractedFiles << path + "/" + file;
	}
	
	Input input = Input::fromList(extractedFiles);
	Options opts = Options::load("/etc/botui/platform.hints");
	Engine engine(Compilers::instance()->compilers());
	OutputList ret = engine.compile(input, opts);
	
	QString firstTerminalFile;
	unsigned int terminals = 0;
	bool success = true;
	foreach(const Output& out, ret) {
		if(out.isTerminal() && out.generatedFiles().size() == 1) {
			if(!terminals) firstTerminalFile = out.generatedFiles()[0];
			++terminals;
		}
		
		success &= out.exitCode() == 0 && out.error().isEmpty();
	}
	
	foreach(const Output& out, ret) {
		qDebug() << out.error();
		qDebug() << out.output();
	}
	
	QFile::remove(m_executables.value(name));
	
	if(!success) {
		m_executables.remove(name);
		return ret;
	}
	
	if(!terminals) {
		ret << OutputList() << Output(name, 1,
			QByteArray(), "error: No terminals detected from compilation.");
		return ret;
	}
	
	if(terminals > 1) {
		ret << Output(name, 0,
			"warning: Terminal ambiguity in compilation. " 
			"Running the ouput of this compilation is undefined.", QByteArray());
	}
	
	QString cachedResult = cachePath(name) + "/" + QFileInfo(firstTerminalFile).fileName();
	QFile::remove(cachedResult);
	if(!QFile::copy(firstTerminalFile, cachedResult)) {
		ret << OutputList() << Output(name, 1, QByteArray(),
			("error: Failed to copy \"" + firstTerminalFile
				+ "\" to \"" + cachedResult + "\"").toLatin1());
	}
	
	m_executables[name] = cachedResult;
	
	sync();
	
	return ret;
}

QString KissCompileProvider::tempPath() const
{
	return QDir::tempPath() + "/" + QDateTime::currentDateTime().toString("yyMMddhhmmss") + ".botui";
}

QString KissCompileProvider::cachePath(const QString& name) const
{
	QString ret = QDir::homePath() + "/cache/" + name;
	QDir().mkpath(ret);
	return ret;
}

QString KissCompileProvider::executableFor(const QString& name) const
{
	return m_executables.value(name);
}

void KissCompileProvider::sync()
{
	qDebug() << "Persistent binary caching is not yet implemented.";
}