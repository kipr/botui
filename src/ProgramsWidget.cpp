#include "ProgramsWidget.h"
#include "ui_ProgramsWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "FilesystemProvider.h"
#include "ProgramWidget.h"
#include <QDebug>

ProgramsWidget::ProgramsWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::ProgramsWidget),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this))
{
	ui->setupUi(this);
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle("Programs");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
	
	ui->programs->setModel(m_device->filesystemProvider()->programsItemModel());
	connect(ui->run, SIGNAL(clicked()), SLOT(run()));
}

ProgramsWidget::~ProgramsWidget()
{
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}

void ProgramsWidget::run()
{
	QModelIndexList currents = ui->programs->selectionModel()->selectedIndexes();
	if(currents.size() != 1) return;
	QModelIndex current = currents[0];
	QString program = m_device->filesystemProvider()->programsItemModel()->program(current);
	ProgramWidget *programWidget = new ProgramWidget(program, m_device);
	RootController::ref().presentWidget(programWidget);
	programWidget->start();
}