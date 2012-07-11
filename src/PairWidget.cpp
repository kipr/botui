#include "PairWidget.h"
#include "ui_PairWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include <QDebug>

PairWidget::PairWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::PairWidget),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this))
{
	ui->setupUi(this);
	QAction *cancelAction = m_menuBar->addAction("Cancel");
	connect(cancelAction, SIGNAL(activated()), SLOT(back()));
	m_menuBar->setTitle("Incoming Connection");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
}

PairWidget::~PairWidget()
{
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}

void PairWidget::back()
{
	RootController::ref().dismissWidget();
}