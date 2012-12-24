#include "FileActionCompile.h"

#include "Device.h"
#include "FilesystemProvider.h"

#include "LogDialog.h"
#include "RootController.h"
#include "ConcurrentCompile.h"
#include "Program.h"
#include "ProgramWidget.h"
#include "ProgramsWidget.h"
#include "CompileProvider.h"

#include <kar.hpp>
#include <QFileInfo>
#include <QThreadPool>

#include <QDebug>

FileActionCompile::FileActionCompile()
	: FileAction("Compile", QStringList() << "c" << "cpp" << "cc" << "cxx")
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
	
	// Create a program archive containing the input file
	
	Kiss::KarPtr archive = Kiss::Kar::create();
	QFile inputFile(path);
	if(!inputFile.open(QIODevice::ReadOnly)) {
		qWarning() << "QFile::open failed on" << path;
		return false;
	}
	archive->addFile(input.fileName(), inputFile.readAll());
	inputFile.close();
	
	// Add this program to the virtual filesystem
	
	const QString name = input.completeBaseName();
	device->filesystemProvider()->setProgram(name, archive);
	
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
		device->filesystemProvider()->deleteProgram(name);
		return false;
	}
	
	const QString exec = device->compileProvider()->executableFor(name);
	
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
		log->appendText(postProcess(out.output()));
		log->appendText(postProcess(out.error()));
	}
	
	// This is more so something will be in the log.
	// It's disconcerting when it's entirely empty.
	log->appendText("Compile finished.");
	
	log->setStatus(Compiler::Output::isSuccess(output) ? tr("Success!") : tr("Failure."));
	log->setDismissable(true);
}

QString FileActionCompile::postProcess(const QString &output) const
{
	QString ret = output;
	QRegExp rx("([a-zA-Z]:)?(/[a-zA-Z0-9_.-]+)*([a-zA-Z0-9_-]+\\.[a-zA-Z]+)/?:");
	int pos = 0;
	while ((pos = rx.indexIn(ret, pos)) != -1) {
		const QString path = ret.mid(pos, rx.matchedLength() - 1);
		const QString ins = QFileInfo(path).fileName();
		ret.replace(pos, path.length(), ins);
		pos += ins.length();
	}
	return ret;
}

REGISTER_FILE_ACTION(FileActionCompile)
