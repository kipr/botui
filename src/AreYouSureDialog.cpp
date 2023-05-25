#include "AreYouSureDialog.h"

#include "ui_AreYouSureDialog.h"

AreYouSureDialog::AreYouSureDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AreYouSureDialog) {
    ui->setupUi(this);
}

AreYouSureDialog::~AreYouSureDialog() { delete ui; }

void AreYouSureDialog::setDescription(const QString &description) {
    ui->descr->setPlainText(description);
}

QString AreYouSureDialog::description() const {
    return ui->descr->toPlainText();
}