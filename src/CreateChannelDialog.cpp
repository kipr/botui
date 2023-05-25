#include "CreateChannelDialog.h"
#include "ui_CreateChannelDialog.h"

#include <kipr/camera/camera.hpp>

CreateChannelDialog::CreateChannelDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::CreateChannelDialog) {
    ui->setupUi(this);
}

CreateChannelDialog::~CreateChannelDialog() { delete ui; }

QString CreateChannelDialog::type() const {
    switch (ui->type->currentIndex()) {
    case 0:
        return CAMERA_CHANNEL_TYPE_HSV_KEY;
    case 1:
        return CAMERA_CHANNEL_TYPE_QR_KEY;
    }
    return QString();
}
