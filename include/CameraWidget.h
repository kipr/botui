#ifndef _CAMERAWIDGET_H_
#define _CAMERAWIDGET_H_

#include "StandardWidget.h"

#include <opencv2/opencv.hpp>

namespace Ui
{
	class CameraWidget;
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
	Ui::CameraWidget *ui;
	cv::VideoCapture m_capture;
};

#endif
