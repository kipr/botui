#include "ChannelConfigWidgetFactory.h"

#include "ChannelConfigWidget.h"
#include "HsvChannelConfigWidget.h"

#include <kovan/camera.hpp>

ChannelConfigWidgetFactory::~ChannelConfigWidgetFactory()
{
}

ChannelConfigWidget *ChannelConfigWidgetFactory::create(const QString &type)
{
	if(type == CAMERA_CHANNEL_TYPE_HSV_KEY) return new HsvChannelConfigWidget();
	return 0;
}

ChannelConfigWidgetFactory::ChannelConfigWidgetFactory()
{
}