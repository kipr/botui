#include "DeveloperListWidget.h" 
#include "ui_DeveloperListWidget.h"
#include "StatusBar.h"
#include "Device.h"
#include "SystemUtils.h"
#include "MenuBar.h"
#include "RootController.h"


#include <QDebug>


DeveloperListWidget::DeveloperListWidget(Device* device, QWidget* parent)
    : StandardWidget(device, parent),
        ui(new Ui::DeveloperListWidget)
{
    ui->setupUi(this);
    performStandardSetup(tr("Developer List"));
} 

DeveloperListWidget::~DeveloperListWidget()
{
    delete ui;
}