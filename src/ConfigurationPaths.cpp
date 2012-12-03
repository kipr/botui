#include "ConfigurationPaths.h"

#include <QDir>

ConfigurationPaths::ConfigurationPaths()
{
	QDir().mkpath(channelsPath());
}

QString ConfigurationPaths::channelsPath() const
{
	return "/etc/botui/channels/";
}