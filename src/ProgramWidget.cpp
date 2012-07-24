#include "ProgramWidget.h"
#include "ui_ProgramWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "CompileProvider.h"
#include "LockScreen.h"

#include <QDebug>

ProgramWidget::ProgramWidget(const QString& program, Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::ProgramWidget),
	m_program(program),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this))
{
	ui->setupUi(this);
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle("Program");
	QAction *lock = m_menuBar->addAction("Lock Screen");
	connect(lock, SIGNAL(activated()), SLOT(lock()));
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
	qDebug() << "Program" << m_program;
	ui->programLabel->setText(m_program);
	
	connect(ui->stop, SIGNAL(clicked()), SLOT(stop()));
	connect(ui->stop, SIGNAL(clicked()), SLOT(stop()));
	
	m_time.start();
}

void ProgramWidget::lock()
{
	LockScreen::lock();
}

bool ProgramWidget::start()
{
	QString executable = m_device->compileProvider()->executableFor(m_program);
	if(executable.isEmpty()) return false;
	QProcess *process = new QProcess(this);
	connect(process, SIGNAL(started()), SLOT(started()));
	connect(process,
		SIGNAL(finished(int, QProcess::ExitStatus)),
		SLOT(finished(int, QProcess::ExitStatus)));
	process->start(executable);
	process->waitForStarted();
	ui->console->setProcess(process);
	return true;
}

void ProgramWidget::stop()
{
	QProcess *process = ui->console->process();
	if(!process) return;
	ui->console->setProcess(0);
	process->terminate();
	if(!process->waitForFinished(2000)) process->kill();
	delete process;
}

void ProgramWidget::started()
{
	ui->stop->setEnabled(true);
	m_time.restart();
}

void ProgramWidget::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	ui->stop->setEnabled(false);
	const int msecs = m_time.elapsed();
	ui->console->append(m_program + tr(" finished in %1 seconds").arg(msecs / 1000.0));
}

ProgramWidget::~ProgramWidget()
{
	stop();
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}
