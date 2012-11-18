#include "ManageNetworks.h"

#include "ui_ManageNetworks.h"

ManageNetworks::ManageNetworks(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::ManageNetworks),
	m_device(device)

{
	ui->setupUi(this);
}

ManageNetworks::~ManageNetworks()
{
	delete ui;
}