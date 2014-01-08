#include "FileActionCompile.h"

#include "Device.h"
#include "ArchivesManager.h"

#include "LogDialog.h"
#include "RootController.h"
#include "ConcurrentCompile.h"
#include "Program.h"
#include "ProgramWidget.h"
#include "ProgramsWidget.h"
#include "CompileProvider.h"
#include "CompileHelpers.h"

#include <kar/kar.hpp>
#include <QFileInfo>
#include <QThreadPool>

#include <QDebug>

FileActionCompile::FileActionCompile()
	: FileActionExtension("Compile", QStringList() << "c" << "cpp" << "cc" << "cxx")
{
	qRegisterMetaType<Compiler::OutputList>("Compiler::OutputList");
}

bool FileActionCompile::act(const QString &path, Device *device) const
{
	// Sanity check
	QFileInfo input(path);
	if(!input.isFile()) {
		qWarning() << "We don't know how to compile a non-file";
		return false;
	}
	if(!device->archivesManager()) {
		qWarning() << "No archives manager";
		return false;
	}
	
	// Create a program archive containing the input file
	
	kiss::KarPtr archive = kiss::Kar::create();
	QFile inputFile(path);
	if(!inputFile.open(QIODevice::ReadOnly)) {
		qWarning() << "QFile::open failed on" << path;
		return false;
	}
	archive->addFile(input.fileName(), inputFile.readAll());
	inputFile.close();
	
	// Add this program to the virtual filesystem
	
	const QString name = input.completeBaseName();
	device->archivesManager()->set(name, archive);
	
	// Compile the program
	
	LogDialog logger;
	ConcurrentCompile compiler(name, archive, device);
	compiler.setAutoDelete(false);
	compiler.setUserData(&logger);
	connect(&compiler, SIGNAL(compileStarted(QString, ConcurrentCompile *)),
		SLOT(compileStarted(QString, ConcurrentCompile *)),
		Qt::QueuedConnection);
	connect(&compiler, SIGNAL(compileFinished(Compiler::OutputList, ConcurrentCompile *)),
		SLOT(compileFinished(Compiler::OutputList, ConcurrentCompile *)),
		Qt::QueuedConnection);
	QThreadPool::globalInstance()->start(&compiler);
	RootController::ref().presentDialog(&logger);
	
	if(!Compiler::Output::isSuccess(compiler.output())) {
		// Clean up
		device->archivesManager()->remove(name);
		return false;
	}
	
	// TODO: We're taking advantage of the fact
	// that our program will be listed first and
	// will be selected. Maybe make this a bit
	// more assured in the future.
	RootController::ref().presentWidget(new ProgramsWidget(device));
	
	return true;
}

void FileActionCompile::compileStarted(const QString &name, ConcurrentCompile *compiler)
{
	if(!compiler) {
		qWarning() << "sender is null";
		return;
	}
	
	LogDialog *log = reinterpret_cast<LogDialog *>(compiler->userData());
	log->setStatus(tr("Compiling..."));
}

void FileActionCompile::compileFinished(const Compiler::OutputList &output, ConcurrentCompile *compiler)
{
	if(!compiler) {
		qWarning() << "sender is null";
		return;
	}

	LogDialog *log = reinterpret_cast<LogDialog *>(compiler->userData());
	foreach(const Compiler::Output& out, output) {
		log->appendText(CompileHelpers::postProcess(out.output()));
		log->appendText(CompileHelpers::postProcess(out.error()));
	}
	
	// This is more so something will be in the log.
	// It's disconcerting when it's entirely empty.
	log->appendText("Compile finished.");
	
	log->setStatus(Compiler::Output::isSuccess(output) ? tr("Success!") : tr("Failure."));
	log->setDismissable(true);
}

REGISTER_FILE_ACTION(FileActionCompile)
