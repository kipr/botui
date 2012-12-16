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
#include "ProgramArgsWidget.h"
#include "ProgramArguments.h"
#include <QDebug>

ProgramsWidget::ProgramsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::ProgramsWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Programs"));
	
	ui->programs->setModel(device->filesystemProvider()->programsItemModel());
	connect(ui->run, SIGNAL(clicked()), SLOT(run()));
	connect(ui->remove, SIGNAL(clicked()), SLOT(remove()));
	connect(ui->args, SIGNAL(clicked()), SLOT(args()));
	
	QModelIndex first = device->filesystemProvider()->programsItemModel()->index(0, 0);
	if(first.isValid()) ui->programs->selectionModel()->select(first, QItemSelectionModel::Select);
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
	QString program = device()->filesystemProvider()->programsItemModel()->program(current);
	QString executable = device()->compileProvider()->executableFor(program);
	if(executable.isEmpty()) {
		qWarning() << "Could not find executable for" << program;
		return;
	}
	
	ProgramWidget *programWidget = new ProgramWidget(Program::instance(), device());
	RootController::ref().presentWidget(programWidget);
	Program::instance()->start(executable,
		ProgramArguments::arguments(device()->filesystemProvider()->program(program)));
}

void ProgramsWidget::args()
{
	QModelIndexList currents = ui->programs->selectionModel()->selectedIndexes();
	if(currents.size() != 1) return;
	QModelIndex current = currents[0];
	QString program = device()->filesystemProvider()->programsItemModel()->program(current);
	RootController::ref().presentWidget(new ProgramArgsWidget(program, device()));
}

void ProgramsWidget::remove()
{
	QModelIndexList currents = ui->programs->selectionModel()->selectedIndexes();
	if(currents.size() != 1) return;
	QModelIndex current = currents[0];
	QString program = device()->filesystemProvider()->programsItemModel()->program(current);
	device()->filesystemProvider()->deleteProgram(program);
}