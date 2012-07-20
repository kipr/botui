#include "CameraWidget.h"
#include "ui_CameraWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include <QDebug>
#include <QTimer>

#include <opencv2/opencv.hpp>

CameraWidget::CameraWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::CameraWidget),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this)),
	m_capture(0)
{
	ui->setupUi(this);
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle("Camera");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
	
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), SLOT(updateCamera()));
	timer->start(50); // 20 FPS
	
	m_capture = cvCaptureFromCAM(CV_CAP_ANY);
	if(m_capture) {
		cvSetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH, 320);
		cvSetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT, 240);
	} else ui->camera->updateImage(0); // Displays no camera message
}

void CameraWidget::updateCamera()
{
	if(!m_capture) return;
	IplImage *image = cvQueryFrame(m_capture);
	if(!image) {
		qWarning() << "cvQueryFrame Failed";
		return;
	}
	ui->camera->updateImage(image);
}

CameraWidget::~CameraWidget()
{
	cvReleaseCapture(&m_capture);
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}
