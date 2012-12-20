#ifndef _CHANNELCONFIGURATIONSWIDGET_H_
#define _CHANNELCONFIGURATIONSWIDGET_H_

#include "StandardWidget.h"

#include <QModelIndex>

namespace Ui
{
	class ChannelConfigurationsWidget;
}

class QFileSystemModel;

class ChannelConfigurationsWidget : public StandardWidget
{
Q_OBJECT
public:
	ChannelConfigurationsWidget(Device *device, QWidget *parent = 0);
	~ChannelConfigurationsWidget();
	
private slots:
	void edit();
	void rename();
	void add();
	void remove();
	
	void currentChanged(const QModelIndex &index);
	
private:
	Ui::ChannelConfigurationsWidget *ui;
	QFileSystemModel *m_model;
};

#endif
