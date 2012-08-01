#include "FactoryWidget.h"
#include "ui_FactoryWidget.h"

#include "Device.h"
#include "NumpadDialog.h"
#include "RootController.h"

FactoryWidget::FactoryWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	m_device(device),
	m_serialNumpad(new NumpadDialog(tr("Serial Number"), NumpadDialog::Integer)),
	ui(new Ui::FactoryWidget)
{
	ui->setupUi(this);
	
	ui->serialNumber->setInputProvider(m_serialNumpad);
	connect(ui->confirm, SIGNAL(clicked()), SLOT(confirm()));
}

FactoryWidget::~FactoryWidget()
{
	delete m_serialNumpad;
	delete ui;
}

void FactoryWidget::confirm()
{
	RootController::ref().dismissWidget();
}