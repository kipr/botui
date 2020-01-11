#ifndef _CAMERALIVEWIDGET_H_
#define _CAMERALIVEWIDGET_H_

#include "StandardWidget.h"

#include <QModelIndex>

namespace Ui
{
	class CameraLiveWidget;
}

class ChannelConfigurationsModel;

class CameraLiveWidget : public StandardWidget
{
Q_OBJECT
public:
	CameraLiveWidget(Device *device, QWidget *parent = 0);
	~CameraLiveWidget();

	//bool isDefaultPath(const QModelIndex &index) const;
	
public slots:
	void currentIndexChanged(const int &index);
	void update();
	
private:
	Ui::CameraLiveWidget *ui;
	ChannelConfigurationsModel *m_model;
	
	//CameraConfigModel *m_channel;
	//QString m_path;
	//QAction *m_import;
	//QString m_defaultPath;
};

#endif
