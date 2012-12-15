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
	ui(new Ui::CameraWidget)
{
	ui->setupUi(this);
	performStandardSetup(tr("Camera"));
	QAction *toggleUi = menuBar()->addAction(tr("Hide UI"));
	connect(toggleUi, SIGNAL(activated()), SLOT(toggleUi()));
	toggleUi->connect(ui->camera, SIGNAL(pressed()), SLOT(trigger()));
	
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), SLOT(updateCamera()));
	timer->start(75); // 15 FPS
	
	m_capture.open(CV_CAP_ANY);
	
	if(!m_capture.isOpened()) return;
	m_capture.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
	m_capture.set(CV_CAP_PROP_FPS, 15);
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

void CameraWidget::toggleUi()
{
	const bool toggle = !statusBar()->isVisible();
	statusBar()->setVisible(toggle);
	const int margin = toggle ? 6 : 0;
	layout()->setContentsMargins(margin, margin, margin, 0);
	layout()->setMenuBar(toggle ? menuBar() : 0);
}

CameraWidget::~CameraWidget()
{
	m_capture.release();
	delete ui;
}
