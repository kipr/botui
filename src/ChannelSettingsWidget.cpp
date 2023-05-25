#include "ChannelSettingsWidget.h"
#include "ui_ChannelSettingsWidget.h"

#include "NumpadDialog.h"

ChannelSettingsWidget::ChannelSettingsWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent), ui(new Ui::ChannelSettingsWidget) {
    ui->setupUi(this);
    performStandardSetup(tr("Channel Settings"));

    ui->minBlobSizeEdit->setInputProvider(
        new NumpadDialog(tr("Min Blob Size"), NumpadDialog::Integer, 0, 500));
}

ChannelSettingsWidget::~ChannelSettingsWidget() {
    emit configChanged(m_index, m_config);
    delete ui;
}

void ChannelSettingsWidget::setConfig(const Config &config) {
    m_config = config;
}

const Config &ChannelSettingsWidget::config() const { return m_config; }

void ChannelSettingsWidget::setIndex(const QModelIndex &index) {
    m_index = index;
}

const QModelIndex &ChannelSettingsWidget::index() const { return m_index; }
