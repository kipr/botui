#include "NotYetImplementedDialog.h"
#include "ui_NotYetImplementedDialog.h"

#include "RootController.h"

NotYetImplementedDialog::NotYetImplementedDialog(QQuickWidget *parent)
	: QDialog(parent),
	ui(new Ui::NotYetImplementedDialog)
{
	ui->setupUi(this);
}

NotYetImplementedDialog::~NotYetImplementedDialog()
{
	delete ui;
}

void NotYetImplementedDialog::nyi()
{
	NotYetImplementedDialog dlg;
	RootController::ref().presentDialog(&dlg);
}
