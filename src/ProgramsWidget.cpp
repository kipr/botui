#include "ProgramsWidget.h"
#include "ui_ProgramsWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "FilesystemProvider.h"
#include "CompileProvider.h"
#include "ProgramWidget.h"
#include "Program.h"
#include <QDebug>

ProgramsWidget::ProgramsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::ProgramsWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Programs"));
	
	ui->programs->setModel(m_device->filesystemProvider()->programsItemModel());
	connect(ui->run, SIGNAL(clicked()), SLOT(run()));
	connect(ui->remove, SIGNAL(clicked()), SLOT(remove()));
}

ProgramsWidget::~ProgramsWidget()
{
	delete ui;
}

void ProgramsWidget::run()
{
	QModelIndexList currents = ui->programs->selectionModel()->selectedIndexes();
	if(currents.size() != 1) return;
	
	QModelIndex current = currents[0];
	QString program = m_device->filesystemProvider()->programsItemModel()->program(current);
	QString executable = m_device->compileProvider()->executableFor(program);
	if(executable.isEmpty()) {
		qWarning() << "Could not find executable for" << program;
		return;
	}
	
	ProgramWidget *programWidget = new ProgramWidget(Program::instance(), m_device);
	RootController::ref().presentWidget(programWidget);
	Program::instance()->start(executable);
}

void ProgramsWidget::remove()
{
	QModelIndexList currents = ui->programs->selectionModel()->selectedIndexes();
	if(currents.size() != 1) return;
	QModelIndex current = currents[0];
	QString program = m_device->filesystemProvider()->programsItemModel()->program(current);
	m_device->filesystemProvider()->deleteProgram(program);
}