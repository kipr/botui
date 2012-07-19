#include "ProgramWidget.h"
#include "ui_ProgramWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
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

ProgramWidget::~ProgramWidget()
{
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}
