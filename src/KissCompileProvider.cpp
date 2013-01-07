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

Compiler::OutputList KissCompileProvider::compile(const QString &name, const Kiss::KarPtr &program)
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
	
	QStringList extracted;
	foreach(const QString& file, program->files()) extracted << path + "/" + file;
	
	Input input = Input::fromList(extracted);
	Options opts = Options::load("/etc/kovan/platform.hints");
	Engine engine(Compilers::instance()->compilers());
	OutputList ret = engine.compile(input, opts);
	
	QStringList terminalFiles;
	bool success = true;
	foreach(const Output& out, ret) {
		QTextStream stream(stdout);
		out.dump(stream);
		if(out.isTerminal() && out.generatedFiles().size() == 1) {
			terminalFiles << out.generatedFiles()[0];
		}
		success &= out.isSuccess();
	}
	
	QString result = QDir(binariesPath()).filePath(name);
	QFile::remove(result);
	
	if(!success) return ret;
	
	if(terminalFiles.isEmpty()) {
		ret << OutputList() << Output(name, 1,
			QByteArray(), "error: No terminals detected from compilation.");
		return ret;
	}
	
	if(terminalFiles.size() > 1) {
		qWarning() << "Multiple Terminals:" << terminalFiles;
		ret << Output(name, 1, QByteArray(), "error: Terminal ambiguity in compilation. ");
	}
	
	if(!QFile::copy(terminalFiles[0], result)) {
		ret << OutputList() << Output(name, 1, QByteArray(),
			("error: Failed to copy \"" + terminalFiles[0]
				+ "\" to \"" + result + "\"").toLatin1());
	}
	
	return ret;
}

QString KissCompileProvider::tempPath() const
{
	return QDir::tempPath() + "/" + QDateTime::currentDateTime().toString("yyMMddhhmmss") + ".botui";
}