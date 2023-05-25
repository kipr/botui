#include "CameraSettingsWidget.h"
#include "ui_CameraSettingsWidget.h"

#include "Device.h"
#include "SettingsProvider.h"

#define BOUNDING_BOX_KEY "bounding_box"
#define NUM_BLOB_LABELS_KEY "num_blob_labels"
#define BLOB_LABELS_KEY "blob_labels"

CameraSettingsWidget::CameraSettingsWidget(Device *device,
                                           QWidget *const parent)
    : StandardWidget(device, parent), ui(new Ui::CameraSettingsWidget) {
    ui->setupUi(this);
    performStandardSetup(tr("Camera Settings Widget"));

    connect(ui->plus, SIGNAL(clicked()), SLOT(plus()));
    connect(ui->minus, SIGNAL(clicked()), SLOT(minus()));

    // Set current setting values
    const SettingsProvider *const settingsProvider = device->settingsProvider();
    if (settingsProvider) {
        const bool bbox =
            settingsProvider->value(BOUNDING_BOX_KEY, true).toBool();
        const bool blobLabels =
            settingsProvider->value(BLOB_LABELS_KEY, true).toBool();
        const int numLabels =
            settingsProvider->value(NUM_BLOB_LABELS_KEY, 0).toInt();

        ui->boundingBox->setChecked(bbox);
        ui->blobLabels->setChecked(blobLabels);
        ui->numLabels->display(numLabels);
    }
}

CameraSettingsWidget::~CameraSettingsWidget() {
    SettingsProvider *const settingsProvider = device()->settingsProvider();
    if (settingsProvider) {
        settingsProvider->setValue(BOUNDING_BOX_KEY,
                                   ui->boundingBox->isChecked());
        settingsProvider->setValue(BLOB_LABELS_KEY,
                                   ui->blobLabels->isChecked());
        settingsProvider->setValue(NUM_BLOB_LABELS_KEY,
                                   ui->numLabels->intValue());
        settingsProvider->sync();
    }

    delete ui;
}

void CameraSettingsWidget::plus() {
    ui->numLabels->display(ui->numLabels->intValue() + 1);
}

void CameraSettingsWidget::minus() {
    ui->numLabels->display(ui->numLabels->intValue() - 1);
}
