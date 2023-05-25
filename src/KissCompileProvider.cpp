#include "KissCompileProvider.h"

#include <pcompiler/pcompiler.hpp>

#include "CompilingWidget.h"
#include "Device.h"
#include "RootController.h"
#include "SystemPrefix.h"

#include <QDateTime>
#include <QDir>
#include <QFile>

#include <QDebug>
#include <QSettings>

#define KISS_COMPILE_GROUP "kiss_compile"
#define EXECUTABLES_KEY "executables"

using namespace Compiler;

KissCompileProvider::KissCompileProvider(Device *device, QObject *parent)
    : CompileProvider(parent), m_device(device) {}

Compiler::OutputList KissCompileProvider::compile(const QString &name,
                                                  const kiss::KarPtr &program,
                                                  const QString &install_dir) {
    if (program.isNull()) {
        return OutputList()
               << Output(name, 1, QByteArray(), "error: KarPtr is null");
    }

    const QString path = tempPath();
    qDebug() << path;

    QMap<QString, QString> settings;
    if (!program->extract(path)) {
        return OutputList() << Output(name,
                                      1,
                                      QByteArray(),
                                      "error: Failed to extract KISS Archive.");
    }

    QStringList extracted;
    foreach (const QString &file, program->files())
        extracted << path + "/" + file;

    Compiler::Input input(extracted.begin(), extracted.end());
    Options opts = Options::load("/etc/kovan/platform.hints");
    opts.setVariable("${USER_ROOT}", "/kovan");
    Engine engine(Compilers::instance()->compilers());
    OutputList ret = engine.compile(input, opts);

    ret << SystemPrefix::ref().rootManager()->install(ret, name);

    // copy the output of the install to the kipr executable place
    QString output = "/kovan/bin/";
    output += name + "/" + name;
    QString dest_file = install_dir + "/bin/botball_user_program";
    qDebug() << "installing to kipr project dir by copying from " << output
             << " to " << dest_file;
    if (QFile::exists(dest_file)) {
        QFile::remove(dest_file);
    }
    if (!QFile::copy(output, dest_file)) {
        qDebug() << "failed to copy";
    }

    return ret;
}

QString KissCompileProvider::tempPath() const {
    return QDir::tempPath() + "/" +
           QDateTime::currentDateTime().toString("yyMMddhhmmss") + ".botui";
}
