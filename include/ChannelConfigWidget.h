#ifndef _CHANNELCONFIGWIDGET_H_
#define _CHANNELCONFIGWIDGET_H_

#include <QWidget>
#include <kovan/config.hpp>

class ChannelConfigWidget : public QWidget
{
Q_OBJECT
public:
	ChannelConfigWidget(QWidget *parent = 0);
	virtual ~ChannelConfigWidget();
	
	void setConfig(const Config &config);
	const Config &config() const;
	
	virtual void refresh() = 0;
	
private:
	Config m_config;
};

#endif
