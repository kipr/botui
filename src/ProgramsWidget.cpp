#include "ProgramsWidget.h"
#include "ui_ProgramsWidget.h"
#include "MenuBar.h"
#include "AreYouSureDialog.h"
#include "RootController.h"
#include "StatusBar.h"
#include "EditorWidget.h"
#include "Device.h"
#include "ArchivesManager.h"
#include "CompileProvider.h"
#include "ArchivesModel.h"
#include "ProgramWidget.h"
#include "KeyboardDialog.h"
#include "Program.h"
#include "LogDialog.h"
#include "ConcurrentCompile.h"
#include "ProgramArgsWidget.h"
#include "ProgramArguments.h"
#include "CompileHelpers.h"

#include <QItemDelegate>
#include <QThreadPool>
#include <QDebug>

class ItemDelegate : public QItemDelegate
{
public:
	ItemDelegate(QObject *parent = 0)
		: QItemDelegate(parent)
	{
	}
	
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		return QSize(0, 22);
	}
};

ProgramsWidget::ProgramsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::ProgramsWidget),
	m_model(new ArchivesModel(device, this))
{
	ui->setupUi(this);
	performStandardSetup(tr("Programs"));
	
	ui->programs->setItemDelegate(new ItemDelegate(this));
	ui->programs->setModel(m_model);
	connect(ui->run, SIGNAL(clicked()), SLOT(run()));
	connect(ui->edit, SIGNAL(clicked()), SLOT(edit()));
	connect(ui->add, SIGNAL(clicked()), SLOT(add()));
	connect(ui->remove, SIGNAL(clicked()), SLOT(remove()));
	connect(ui->args, SIGNAL(clicked()), SLOT(args()));
	
	connect(ui->programs->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
		SLOT(update()));
	
	update();
}

ProgramsWidget::~ProgramsWidget()
{
	delete ui;
}

void ProgramsWidget::run()
{
	QModelIndexList currents = ui->programs->selectionModel()->selectedIndexes();
	if(currents.size() != 1) return;
	
	const QString name = m_model->name(currents[0]);
	
	if(!device()->archivesManager()->hasBinary(name)) {
		qWarning() << "Could not find executable for" << name << ". Trying to compile.";
		LogDialog logger;
		ConcurrentCompile compiler(name, device()->archivesManager()->archive(name),
			device());
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
		if(!Compiler::Output::isSuccess(compiler.output())) return;
	}
	
	ProgramWidget *programWidget = new ProgramWidget(Program::instance(), device());
	if(!Program::instance()->start(device()->archivesManager()->binaryPath(name),
		ProgramArguments::arguments(device()->archivesManager()->archive(name)))) {
		delete programWidget;
	} else {
		RootController::ref().presentWidget(programWidget);
	}
}

void ProgramsWidget::edit()
{
	QModelIndexList currents = ui->programs->selectionModel()->selectedIndexes();
	if(currents.size() != 1) return;
	
	const QString name = m_model->name(currents[0]);
	Kiss::KarPtr archive = device()->archivesManager()->archive(name);
	if(archive.isNull()) return;
	
	EditorWidget *editor = new EditorWidget(device());
	editor->setArchive(archive);
	editor->setSavePath(device()->archivesManager()->archivePath(name));
	RootController::ref().presentWidget(editor);
}

void ProgramsWidget::add()
{
	KeyboardDialog keyboard(tr("Program Name"));
	if(RootController::ref().presentDialog(&keyboard) != QDialog::Accepted) return;
	const QString name = keyboard.input();
	if(name.isEmpty()) return;
	
	if(!device()->archivesManager()->archive(name).isNull()) {
		AreYouSureDialog dialog;
		dialog.setDescription(tr("You're about to overwrite the program \"%1\". Continue?\n").arg(name));
		if(RootController::ref().presentDialog(&dialog) != QDialog::Accepted) return;
	}
	
	Kiss::KarPtr archive = Kiss::Kar::create();
	if(!device()->archivesManager()->set(name, archive)) {
		qWarning() << "Failed to create new archive with the name" << name;
		return;
	}
	
	EditorWidget *editor = new EditorWidget(device());
	editor->setArchive(archive);
	editor->setSavePath(device()->archivesManager()->archivePath(name));
	RootController::ref().presentWidget(editor);
}

void ProgramsWidget::args()
{
	QModelIndexList currents = ui->programs->selectionModel()->selectedIndexes();
	if(currents.size() != 1) return;
	const QString name = m_model->name(currents[0]);
	RootController::ref().presentWidget(new ProgramArgsWidget(name, device()));
}

void ProgramsWidget::remove()
{
	QModelIndexList currents = ui->programs->selectionModel()->selectedIndexes();
	if(currents.size() != 1) return;
	device()->archivesManager()->remove(m_model->name(currents[0]));
	update();
}

void ProgramsWidget::compileStarted(const QString &name, ConcurrentCompile *compiler)
{
	if(!compiler) {
		qWarning() << "sender is null";
		return;
	}
	
	LogDialog *log = reinterpret_cast<LogDialog *>(compiler->userData());
	log->setStatus(tr("Compiling..."));
}

void ProgramsWidget::compileFinished(const Compiler::OutputList &output, ConcurrentCompile *compiler)
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
	log->appendText(tr("Compile finished."));
	
	log->setStatus(Compiler::Output::isSuccess(output) ? tr("Success!") : tr("Failure."));
	log->setDismissable(true);
}

void ProgramsWidget::update()
{
	QModelIndexList currents = ui->programs->selectionModel()->selectedIndexes();
	const bool good = currents.size() == 1;
	ui->run->setEnabled(good);
	ui->edit->setEnabled(good);
	ui->remove->setEnabled(good);
	ui->args->setEnabled(good);
}