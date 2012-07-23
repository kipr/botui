#include "ProgramWidget.h"
#include "ui_ProgramWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "CompileProvider.h"

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
	m_menuBar->addAction("Lock Screen");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
	qDebug() << "Program" << m_program;
	ui->programLabel->setText(m_program);
}

void ProgramWidget::start()
{
	QProcess *process = new QProcess(this);
	process->start(m_device->compileProvider()->executableFor(m_program));
	process->waitForStarted();
	ui->console->setProcess(process);
}

void ProgramWidget::stop()
{
	QProcess *process = ui->console->process();
	if(!process) return;
	process->terminate();
	if(!process->waitForFinished(2000)) process->kill();
	delete process;
}

ProgramWidget::~ProgramWidget()
{
	stop();
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}
