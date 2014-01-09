#include "KissCompileProvider.h"

#include <pcompiler/pcompiler.hpp>

#include "RootController.h"
#include "CompilingWidget.h"
#include "Device.h"
#include "SystemPrefix.h"

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

Compiler::OutputList KissCompileProvider::compile(const QString &name, const kiss::KarPtr &program)
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
  opts.setVariable("USER_ROOT", "/kovan");
	Engine engine(Compilers::instance()->compilers());
	OutputList ret = engine.compile(input, opts);
	
  ret << SystemPrefix::ref().rootManager()->install(ret, name);
  return ret;
}

QString KissCompileProvider::tempPath() const
{
	return QDir::tempPath() + "/" + QDateTime::currentDateTime().toString("yyMMddhhmmss") + ".botui";
}