#include "ConnectWidget.h"
#include "ui_ConnectWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "NetworkingProvider.h"
#include "Device.h"
#include <QDebug>

ConnectWidget::ConnectWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::ConnectWidget),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this))
{
	ui->setupUi(this);
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle(tr("Connect"));
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
	ui->networks->setModel(m_device->networkingProvider()->networkItemModel());
	
	QObject::connect(ui->refresh, SIGNAL(clicked()), SLOT(refresh()));
	QObject::connect(ui->connect, SIGNAL(clicked()), SLOT(connect()));
}

ConnectWidget::~ConnectWidget()
{
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}

void ConnectWidget::connect()
{
	QModelIndexList selection = ui->networks->selectionModel()->selectedIndexes();
	if(selection.size() != 1) return;
	NetworkPtr network = m_device->networkingProvider()->networkItemModel()->network(selection[0]);
	network->connect();
	qDebug() << network->security();
}

void ConnectWidget::other()
{
	
}

void ConnectWidget::refresh()
{
	m_device->networkingProvider()->scan();
}