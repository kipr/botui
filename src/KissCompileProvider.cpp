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
	
  OutputList ret;
  return ret;
}

QString KissCompileProvider::tempPath() const
{
	return QDir::tempPath() + "/" + QDateTime::currentDateTime().toString("yyMMddhhmmss") + ".botui";
}
