#include "KissIdeSettingsWidget.h"
#include "ui_KissIdeSettingsWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include <QDebug>

KissIdeSettingsWidget::KissIdeSettingsWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::KissIdeSettingsWidget),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this))
{
	ui->setupUi(this);
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle("KISS IDE Settings");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
}

KissIdeSettingsWidget::~KissIdeSettingsWidget()
{
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}
