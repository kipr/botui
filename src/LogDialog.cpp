#include "LogDialog.h"

#include "ui_LogDialog.h"

#include <QPushButton>

LogDialog::LogDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LogDialog) {
    ui->setupUi(this);
    setDismissable(false);
}

void LogDialog::setDismissable(const bool &dismissable) {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(dismissable);
}

void LogDialog::setStatus(const QString &status) {
    ui->status->setText(status);
}

void LogDialog::appendText(const QString &text) {
    ui->log->insertPlainText(text);
}

void LogDialog::clearText() { ui->log->clear(); }