#include "AccessoriesWidget.h"
#include "ui_AccessoriesWidget.h"

#include "CreateWidget.h"
#include "RootController.h"

AccessoriesWidget::AccessoriesWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent), ui(new Ui::AccessoriesWidget) {
    ui->setupUi(this);
    performStandardSetup(tr("Accessories"));
}

AccessoriesWidget::~AccessoriesWidget() { delete ui; }

void AccessoriesWidget::create() {
    RootController::ref().presentWidget(new CreateWidget(device()));
}

void AccessoriesWidget::ardrone() {}