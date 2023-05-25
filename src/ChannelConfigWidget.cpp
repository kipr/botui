#include "ChannelConfigWidget.h"

ChannelConfigWidget::ChannelConfigWidget(const QModelIndex &index,
                                         QWidget *parent)
    : QWidget(parent), m_index(index) {}

ChannelConfigWidget::~ChannelConfigWidget() {}

void ChannelConfigWidget::setConfig(const Config &config) {
    m_config = config;
    refresh();
    emit configChanged(m_index, m_config);
}

const Config &ChannelConfigWidget::config() const { return m_config; }
