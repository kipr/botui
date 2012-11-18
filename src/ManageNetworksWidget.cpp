#include "ManageNetworksWidget.h"

#include "ui_ManageNetworks.h"

#include "NetworkManager.h"
#include "NetworkItemModel.h"

ManageNetworksWidget::ManageNetworksWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::ManageNetworks),
	m_device(device),
	m_model(new NetworkItemModel(this))
{
	ui->setupUi(this);
	ui->networks->setModel(m_model);
	
	m_model->setNetworks(NetworkManager::ref().networks());
	
	connect(ui->forget, SIGNAL(clicked()), SLOT(forget()));
	connect(ui->info, SIGNAL(clicked()), SLOT(information()));
}

ManageNetworksWidget::~ManageNetworksWidget()
{
	delete ui;
}

void ManageNetworksWidget::forget()
{
	// TODO
}

void ManageNetworksWidget::information()
{
	// TODO
}