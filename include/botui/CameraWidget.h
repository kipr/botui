#ifndef _CAMERAWIDGET_H_
#define _CAMERAWIDGET_H_

#include "StandardWidget.h"

#include <QTimer>

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
	void updateImage();
	void currentIndexChanged(const int &index);
	
private:
	Ui::CameraWidget *ui;
	Camera::Device *m_device;
	ChannelConfigurationsModel *m_model;
  QTimer *m_timer;
};

#endif
