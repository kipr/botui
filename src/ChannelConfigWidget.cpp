#include "ChannelConfigWidget.h"

ChannelConfigWidget::ChannelConfigWidget(QWidget *parent)
	: QWidget(parent)
{
}

ChannelConfigWidget::~ChannelConfigWidget()
{
}

void ChannelConfigWidget::setConfig(const Config &config)
{
	m_config = config;
	refresh();
}

const Config &ChannelConfigWidget::config() const
{
	return m_config;
}