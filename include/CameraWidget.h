#ifndef _CAMERAWIDGET_H_
#define _CAMERAWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
	class CameraWidget;
}

namespace Camera
{
	class Device;
}

class ChannelConfigurationsModel;

class CameraWidget : public StandardWidget
{
Q_OBJECT
public:
	CameraWidget(Device *device, QWidget *parent = 0);
	~CameraWidget();
	
public slots:
	void updateCamera();
	void currentIndexChanged(const int &index);
	
private:
	void setUpdateFast();
	void setUpdateSlow();
	
	Ui::CameraWidget *ui;
	Camera::Device *m_device;
	QTimer *m_timer;
	ChannelConfigurationsModel *m_model;
};

#endif
