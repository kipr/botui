#include "CommunicationSettingsWidget.h"
#include "ui_CommunicationSettingsWidget.h"

#include "KeyboardDialog.h"

#include <kovan/config.hpp>

#include <QString>

#define SETTINGS_FILE "/etc/kovan/device.conf"
#define DEVICE_NAME_KEY "device_name"

CommunicationSettingsWidget::CommunicationSettingsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::CommunicationSettingsWidget())
{
	ui->setupUi(this);
	performStandardSetup(tr("Communication Settings"));
	ui->deviceName->setInputProvider(new KeyboardDialog(tr("Device Name"), KeyboardDialog::Normal, this));
	
	Config *settings = Config::load(SETTINGS_FILE);
	if(settings) {
		QString name = QString::fromStdString(settings->stringValue(DEVICE_NAME_KEY));
		ui->deviceName->setText(name);
	}
	delete settings;
	
	connect(ui->deviceName, SIGNAL(textChanged(QString)),
		SLOT(deviceNameChanged(QString)));
}

CommunicationSettingsWidget::~CommunicationSettingsWidget()
{
}

void CommunicationSettingsWidget::deviceNameChanged(const QString &text)
{
	Config *settings = Config::load(SETTINGS_FILE);
	if(!settings) settings = new Config();
	settings->setValue(DEVICE_NAME_KEY, ui->deviceName->text().toStdString());
	// TODO: Error checking?
	settings->save(SETTINGS_FILE);
	delete settings;
}