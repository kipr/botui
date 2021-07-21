#ifndef _CHANNELCONFIGWIDGET_H_
#define _CHANNELCONFIGWIDGET_H_

#include <QQuickWidget>

#ifdef WALLABY
#include <wallaby/config.hpp>
#else
#include <kovan/config.hpp>
#endif


#include <QModelIndex>

class ChannelConfigWidget : public QQuickWidget
{
Q_OBJECT
public:
	ChannelConfigWidget(const QModelIndex &index, QQuickWidget *parent = 0);
	virtual ~ChannelConfigWidget();
	
	Q_PROPERTY(Config config READ config WRITE setConfig NOTIFY configChanged)
	void setConfig(const Config &config);
	const Config &config() const;
	
	virtual void refresh() = 0;
	
signals:
	void configChanged(const QModelIndex &index, const Config &config);
	
private:
	Config m_config;
	QModelIndex m_index;
};

#endif
