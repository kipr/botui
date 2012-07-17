#ifndef _CAMERAWIDGET_H_
#define _CAMERAWIDGET_H_

#include <QWidget>

namespace Ui
{
	class CameraWidget;
}

class MenuBar;
class StatusBar;
class Device;
struct CvCapture;

class CameraWidget : public QWidget
{
Q_OBJECT
public:
	CameraWidget(Device *device, QWidget *parent = 0);
	~CameraWidget();
	
public slots:
	void updateCamera();
	
private:
	Ui::CameraWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
	CvCapture *m_capture;
};

#endif
