#include "ServosWidget.h"
#include "ui_ServosWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include <QDebug>

ServosWidget::ServosWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::ServosWidget),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this))
{
	ui->setupUi(this);
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle("Servos");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
}

ServosWidget::~ServosWidget()
{
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}
