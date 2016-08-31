#include "ProgramsWidget.h"
#include "ui_ProgramsWidget.h"
#include "MenuBar.h"
#include "AreYouSureDialog.h"
#include "RootController.h"
#include "StatusBar.h"
#include "EditorWidget.h"
#include "Device.h"
#include "SystemPrefix.h"
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
#include <QTimer>
#include <QMessageBox>
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
	connect(ui->transfer, SIGNAL(clicked()), SLOT(transfer()));
	
	// TODO: remove these once the buttons all work
	ui->edit->setVisible(false);
	ui->add->setVisible(false);
	ui->remove->setVisible(false);
	ui->args->setVisible(false);
	ui->transfer->setVisible(false);

	connect(ui->programs->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
		SLOT(update()));
	
  QTimer *timer = new QTimer(this);
  timer->start(2500);
  connect(timer, SIGNAL(timeout()), SLOT(update()));
  
	update();
}

ProgramsWidget::~ProgramsWidget()
{
	delete ui;
}

void ProgramsWidget::run()
{
  if(Program::instance()->isRunning()) return;
  
	QModelIndexList currents = ui->programs->selectionModel()->selectedIndexes();
	if(currents.size() != 1) return;
  
  //ui->run->setEnabled(false);
	
	const QString name = m_model->name(currents[0]);
	
  // TODO: removed attempt to compile
	/*if(!QFileInfo(SystemPrefix::ref().rootManager()->binPath(name)).exists()) {
		qWarning() << "Could not find executable for" << name << ". Trying to compile.";
		LogDialog logger;
    const QString archivePath = SystemPrefix::ref().rootManager()->archivesPath(name);
  	kiss::KarPtr archive = kiss::Kar::load(archivePath);
		ConcurrentCompile compiler(name, archive, device());
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
      ui->run->setEnabled(true);
      return;
    }
	}*/
  // TODO: hardcoded system path
  // Make sure binary exists for this project
  const QDir projDir("/home/kipr/Documents/KISS/" + name);
  qDebug() << name;
  if(!projDir.exists("bin/botball_user_program")) {
    QMessageBox::warning(this, tr("No Executable"), tr("No executable exists for the selected proejct."));
    return;
  }
  
  ui->run->setEnabled(false);
  
  // TODO: removed attempt to compile
  /*const QString archivePath = SystemPrefix::ref().rootManager()->archivesPath(name);
	ProgramWidget *programWidget = new ProgramWidget(Program::instance(), device());
  kiss::KarPtr archive = kiss::Kar::load(archivePath);
  bool success = true;
	if(!(success = Program::instance()->start(SystemPrefix::ref().rootManager()->bin(name).filePath(name),
      ProgramArguments::arguments(archive)))) {
		qWarning() << "Failed to start program" << name << ". Trying to compile.";
		LogDialog logger;
		ConcurrentCompile compiler(name, archive, device());
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
		success = Compiler::Output::isSuccess(compiler.output());
    if(success) success = Program::instance()->start(SystemPrefix::ref().rootManager()->bin(name).filePath(name),
      ProgramArguments::arguments(archive));
	}*/
  
  ProgramWidget *programWidget = new ProgramWidget(Program::instance(), device());
  const bool success = Program::instance()->start(projDir.filePath("bin/botball_user_program"), QStringList());
  
  if(success) RootController::ref().presentWidget(programWidget);
  else delete programWidget;
  
  ui->run->setEnabled(true);
}

void ProgramsWidget::edit()
{
	QModelIndexList currents = ui->programs->selectionModel()->selectedIndexes();
	if(currents.size() != 1) return;
	
  
	const QString name = m_model->name(currents[0]);
  const QString archivePath = SystemPrefix::ref().rootManager()->archivesPath(name);
	kiss::KarPtr archive = kiss::Kar::load(archivePath);
	if(archive.isNull()) return;
	
	EditorWidget *editor = new EditorWidget(device());
	editor->setArchive(archive);
	editor->setSavePath(archivePath);
	RootController::ref().presentWidget(editor);
}

void ProgramsWidget::add()
{
	KeyboardDialog keyboard(tr("Program Name"));
	if(RootController::ref().presentDialog(&keyboard) != QDialog::Accepted) return;
	const QString name = keyboard.input();
	if(name.isEmpty()) return;
	
  const QString archivePath = SystemPrefix::ref().rootManager()->archivesPath(name);
	if(!kiss::Kar::load(archivePath).isNull()) {
		AreYouSureDialog dialog;
		dialog.setDescription(tr("You're about to overwrite the program \"%1\". Continue?\n").arg(name));
		if(RootController::ref().presentDialog(&dialog) != QDialog::Accepted) return;
	}
	
	kiss::KarPtr archive = kiss::Kar::create();
	if(!archive->save(archivePath)) {
		qWarning() << "Failed to create new archive with the name" << name;
		return;
	}
	
	EditorWidget *editor = new EditorWidget(device());
	editor->setArchive(archive);
	editor->setSavePath(archivePath);
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
	SystemPrefix::ref().rootManager()->uninstall(m_model->name(currents[0]));
	update();
}
 
void ProgramsWidget::transfer()
{
	QModelIndexList currents = ui->programs->selectionModel()->selectedIndexes();
	if(currents.size() != 1) return;
  const QString name = m_model->name(currents[0]);
	const kiss::KarPtr archive = kiss::Kar::load(SystemPrefix::ref().rootManager()->archivesPath(name));
  const QDir flashDrive("/kovan/media/sda1/transfers/" + name);
  QDir().mkpath(flashDrive.path());
  archive->extract(flashDrive.path());
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
	ui->run->setEnabled(good && !Program::instance()->isRunning());
	ui->edit->setEnabled(good);
	ui->remove->setEnabled(good);
	ui->args->setEnabled(good);
  const QDir flashDrive("/kovan/media/sda1");
	ui->transfer->setEnabled(good && flashDrive.exists());
}
