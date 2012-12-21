#ifndef _CAMERAWIDGET_H_
#define _CAMERAWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
	class CameraWidget;
}

namespace cv
{
	class VideoCapture;
}

class CameraWidget : public StandardWidget
{
Q_OBJECT
public:
	CameraWidget(Device *device, QWidget *parent = 0);
	~CameraWidget();
	
public slots:
	void updateCamera();
	void toggleUi();
	
private:
	void setUpdateFast();
	void setUpdateSlow();
	
	Ui::CameraWidget *ui;
	cv::VideoCapture *m_capture;
	QTimer *m_timer;
};

#endif
