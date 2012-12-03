#ifndef _CHANNELSWIDGET_H_
#define _CHANNELSWIDGET_H_

#include "Channels.h"

#include <QWidget>

namespace Ui
{
	class ChannelsWidget;
}

class ChannelsWidget : public QWidget
{
Q_OBJECT
public:
	ChannelsWidget(QWidget *parent = 0);
	~ChannelsWidget();
	
private slots:
	void save();
	void add();
	void remove();
	void activeChanged(const int &index);
	
private:
	Ui::ChannelsWidget *ui;
	ChannelsPtr m_channels;
};

#endif
