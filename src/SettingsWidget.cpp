#include "SettingsWidget.h"
#include "ui_SettingsWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"

#include "NetworkSettingsWidget.h"
#include "KissIdeSettingsWidget.h"
#include "PackageLandingWidget.h"
#include "StringNumberEditWidget.h"

#include <QDebug>

SettingsWidget::SettingsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::SettingsWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Settings"));
	
	// ui->networkSettings->setEnabled(m_device->networkingProvider());
	ui->packages->setEnabled(m_device->packageManagerProvider());
	
	connect(ui->networkSettings, SIGNAL(clicked()), SLOT(network()));
	connect(ui->kissIdeSettings, SIGNAL(clicked()), SLOT(kissIde()));
	connect(ui->packages, SIGNAL(clicked()), SLOT(packages()));
	connect(ui->stringNumber, SIGNAL(clicked()), SLOT(tmp())); // remove
}

SettingsWidget::~SettingsWidget()
{
	delete ui;
}

void SettingsWidget::network()
{
	RootController::ref().presentWidget(new NetworkSettingsWidget(m_device));
}

void SettingsWidget::kissIde()
{
	RootController::ref().presentWidget(new KissIdeSettingsWidget(m_device));
}

void SettingsWidget::packages()
{
	RootController::ref().presentWidget(new PackageLandingWidget(m_device));
}

void SettingsWidget::tmp() // remove
{
	RootController::ref().presentWidget(new StringNumberEditWidget(m_device));
}