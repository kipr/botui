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
	m_statusBar(new StatusBar(this))
{
	ui->setupUi(this);
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle("Camera");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
	
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), SLOT(updateCamera()));
	timer->start(75); // 15 FPS
	
	m_capture.open(CV_CAP_ANY);
	
	if(!m_capture.isOpened()) return;
	m_capture.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
	m_capture.set(CV_CAP_PROP_FPS, 20);
}

void CameraWidget::updateCamera()
{
	ui->camera->setInvalid(!m_capture.isOpened());
	if(!m_capture.isOpened()) return;
	cv::Mat image;
	if(!m_capture.grab() || !m_capture.retrieve(image)) {
		qWarning() << "grab/retrieve pair failed";
		ui->camera->setInvalid(true);
		m_capture.release();
		return;
	}
	ui->camera->updateImage(image);
}

CameraWidget::~CameraWidget()
{
	m_capture.release();
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}
