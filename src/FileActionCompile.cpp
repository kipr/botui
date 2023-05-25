#include "FileActionCompile.h"

#include "Device.h"
#include "SystemPrefix.h"

#include "CompileHelpers.h"
#include "CompileProvider.h"
#include "ConcurrentCompile.h"
#include "LogDialog.h"
#include "Program.h"
#include "ProgramWidget.h"
#include "ProgramsWidget.h"
#include "RootController.h"

//#include <kar/kar.hpp>
#include <QFileInfo>
#include <QThreadPool>

#include <QDebug>

FileActionCompile::FileActionCompile()
    : FileActionExtension(QObject::tr("Compile"), QStringList() << "kissproj") {
    qRegisterMetaType<Compiler::OutputList>("Compiler::OutputList");
}

bool FileActionCompile::act(const QString &path, Device *device) const {
    // Sanity check
    QFileInfo input(path);
    if (!input.isFile()) {
        qWarning() << "We don't know how to compile a non-file";
        return false;
    }
    /*
    // Create a program archive containing the input file
    kiss::KarPtr archive = kiss::Kar::create(input.path());
  qDebug() << archive->files();
  foreach(QString file, archive->files()) {
    if(!file.endsWith(".kissproj")) continue;
    const QString old = file;
    file.replace(".kissproj", ".ops");
    archive->rename(old, file);
  }

  const QStringList cExts = QStringList() << "c" << "cpp" << "cxx" << "cc";
  bool isCProj = false;
  Q_FOREACH(const QString &file, archive->files()) {
    QFileInfo info(file);
    isCProj |= (bool)cExts.contains(info.completeSuffix(), Qt::CaseInsensitive);
  }

  if(isCProj) {
    QFile file(":/target.c");
    if(!file.open(QIODevice::ReadOnly)) {
      qWarning() << "Failed to inject target.c";
    } else {
      archive->setFile("__internal_target___.c", file.readAll());
      file.close();
    }
  }

  const QString name = input.baseName();
    // Add this program to the virtual filesystem
  const QString archivePath =
  SystemPrefix::ref().rootManager()->archivesPath(name);
  if(!archive->save(archivePath)) {
        qWarning() << "Archive save failed on" << archivePath;
        return false;
  }

    // Compile the program

    LogDialog logger;
    ConcurrentCompile compiler(name, archive, device);
    compiler.setAutoDelete(false);
    compiler.setUserData(&logger);
    connect(&compiler, SIGNAL(compileStarted(QString, ConcurrentCompile *)),
        SLOT(compileStarted(QString, ConcurrentCompile *)),
        Qt::QueuedConnection);
    connect(&compiler, SIGNAL(compileFinished(Compiler::OutputList,
  ConcurrentCompile *)), SLOT(compileFinished(Compiler::OutputList,
  ConcurrentCompile *)), Qt::QueuedConnection);
    QThreadPool::globalInstance()->start(&compiler);
    RootController::ref().presentDialog(&logger);

    if(!Compiler::Output::isSuccess(compiler.output())) {
        // Clean up
        SystemPrefix::ref().rootManager()->uninstall(name);
        return false;
    }

    // TODO: We're taking advantage of the fact
    // that our program will be listed first and
    // will be selected. Maybe make this a bit
    // more assured in the future.
    RootController::ref().presentWidget(new ProgramsWidget(device));
    */
    return true;
}

void FileActionCompile::compileStarted(const QString &name,
                                       ConcurrentCompile *compiler) {
    if (!compiler) {
        qWarning() << "sender is null";
        return;
    }

    LogDialog *log = reinterpret_cast<LogDialog *>(compiler->userData());
    log->setStatus(tr("Compiling..."));
}

void FileActionCompile::compileFinished(const Compiler::OutputList &output,
                                        ConcurrentCompile *compiler) {
    if (!compiler) {
        qWarning() << "sender is null";
        return;
    }

    LogDialog *log = reinterpret_cast<LogDialog *>(compiler->userData());
    foreach (const Compiler::Output &out, output) {
        log->appendText(CompileHelpers::postProcess(out.output()));
        log->appendText(CompileHelpers::postProcess(out.error()));
    }

    // This is more so something will be in the log.
    // It's disconcerting when it's entirely empty.
    log->appendText("Compile finished.");

    log->setStatus(Compiler::Output::isSuccess(output) ? tr("Success!")
                                                       : tr("Failure."));
    log->setDismissable(true);
}

REGISTER_FILE_ACTION(FileActionCompile)
