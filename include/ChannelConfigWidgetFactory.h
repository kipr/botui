#ifndef _CHANNELCONFIGWIDGETFACTORY_H_
#define _CHANNELCONFIGWIDGETFACTORY_H_

#include <QString>

class ChannelConfigWidget;

class ChannelConfigWidgetFactory
{
public:
	~ChannelConfigWidgetFactory();
	
	static ChannelConfigWidget *create(const QString &type);
	
private:
	ChannelConfigWidgetFactory();
};

#endif
