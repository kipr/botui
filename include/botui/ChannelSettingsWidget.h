#ifndef _CHANNELSETTINGSWIDGET_H_
#define _CHANNELSETTINGSWIDGET_H_
/* FIXME
#include "StandardWidget.h"

#ifdef WALLABY
#include <wallaby/config.hpp>
#else
#include <kovan/config.hpp>
#endif

#include <QModelIndex>

namespace Ui
{
	class ChannelSettingsWidget;
}

class ChannelSettingsWidget : public StandardWidget
{
Q_OBJECT
public:
	ChannelSettingsWidget(Device *device, QWidget *parent = 0);
	~ChannelSettingsWidget();
	
	Q_PROPERTY(Config config READ config WRITE setConfig);
	void setConfig(const Config &config);
	const Config &config() const;
	
	Q_PROPERTY(QModelIndex index READ index WRITE setIndex);
	void setIndex(const QModelIndex &index);
	const QModelIndex &index() const;
	
signals:
	void configChanged(const QModelIndex &index, const Config &config);
	
private:
	Ui::ChannelSettingsWidget *ui;
	Config m_config;
	QModelIndex m_index;
};
*/

#endif
