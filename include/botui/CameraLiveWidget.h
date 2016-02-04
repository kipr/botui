#ifndef _CAMERALIVEWIDGET_H_
#define _CAMERALIVEWIDGET_H_

#include "StandardWidget.h"

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
	
public slots:
	void currentIndexChanged(const int &index);
	
private:
	Ui::CameraLiveWidget *ui;
	ChannelConfigurationsModel *m_model;
};

#endif
