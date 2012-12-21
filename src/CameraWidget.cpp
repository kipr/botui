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
	: StandardWidget(device, parent),
	ui(new Ui::CameraWidget),
	m_capture(new cv::VideoCapture),
	m_timer(new QTimer(this))
{
	ui->setupUi(this);
	performStandardSetup(tr("Camera"));
	QAction *toggleUi = menuBar()->addAction(tr("Hide UI"));
	connect(toggleUi, SIGNAL(activated()), SLOT(toggleUi()));
	toggleUi->connect(ui->camera, SIGNAL(pressed()), SLOT(trigger()));
	
	connect(m_timer, SIGNAL(timeout()), SLOT(updateCamera()));
}

CameraWidget::~CameraWidget()
{
	m_capture->release();
	delete ui;
	delete m_capture;
}

void CameraWidget::updateCamera()
{
	if(!m_capture->isOpened()) {
		if(!m_capture->open(CV_CAP_ANY)) {
			setUpdateSlow();
			return;
		}
		m_capture->set(CV_CAP_PROP_FRAME_WIDTH, 320);
		m_capture->set(CV_CAP_PROP_FRAME_HEIGHT, 240);
		m_capture->set(CV_CAP_PROP_FPS, 15);
		ui->camera->setInvalid(false);
		setUpdateFast();
	}
	cv::Mat image;
	if(!m_capture->grab() || !m_capture->retrieve(image)) {
		qWarning() << "grab/retrieve pair failed";
		ui->camera->setInvalid(true);
		m_capture->release();
		setUpdateSlow();
		return;
	}
	ui->camera->updateImage(image);
}

void CameraWidget::toggleUi()
{
	const bool toggle = !statusBar()->isVisible();
	statusBar()->setVisible(toggle);
	const int margin = toggle ? 6 : 0;
	layout()->setContentsMargins(margin, margin, margin, 0);
	layout()->setMenuBar(toggle ? menuBar() : 0);
}

void CameraWidget::setUpdateFast()
{
	m_timer->start(75); // 15 FPS
}

void CameraWidget::setUpdateSlow()
{
	m_timer->start(1000);
}
