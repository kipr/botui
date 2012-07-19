#include "CompilingWidget.h"
#include "ui_CompilingWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include <QDebug>

CompilingWidget::CompilingWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::CompilingWidget),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this))
{
	ui->setupUi(this);
	m_menuBar->setTitle("Compiling...");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
}

CompilingWidget::~CompilingWidget()
{
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}

void CompilingWidget::compileFinished()
{
	
}
