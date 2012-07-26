#include "ConnectWidget.h"
#include "ui_ConnectWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "NetworkingProvider.h"
#include "OtherNetworkWidget.h"
#include "Device.h"
#include <QDebug>

ConnectWidget::ConnectWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::ConnectWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Connect"));
	ui->networks->setModel(m_device->networkingProvider()->networkItemModel());
	
	QObject::connect(ui->refresh, SIGNAL(clicked()), SLOT(refresh()));
	QObject::connect(ui->other, SIGNAL(clicked()), SLOT(other()));
	QObject::connect(ui->connect, SIGNAL(clicked()), SLOT(connect()));
}

ConnectWidget::~ConnectWidget()
{
	delete ui;
}

void ConnectWidget::connect()
{
	QModelIndexList selection = ui->networks->selectionModel()->selectedIndexes();
	if(selection.size() != 1) return;
	NetworkPtr network = m_device->networkingProvider()->networkItemModel()->network(selection[0]);
	network->setAutoConnect(true);
	if(network->security().size() > 0) qDebug() << "This network has security" << network->security();
	if(!network->connect()) {
		qWarning() << "Connection Failed";
	} else RootController::ref().dismissWidget();
}

void ConnectWidget::other()
{
	RootController::ref().presentWidget(new OtherNetworkWidget(m_device));
}

void ConnectWidget::refresh()
{
	m_device->networkingProvider()->scan();
}