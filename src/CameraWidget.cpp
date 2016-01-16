#include "CameraWidget.h"
#include "ui_CameraWidget.h"
#include "Device.h"
#include "ChannelConfigurationsModel.h"
#include "CameraInputSelectorWidget.h"
#include "RootController.h"
#include "CameraInputManager.h"
#include "MenuBar.h"
#include <QDebug>

#ifdef WALLABY
#include <wallaby/camera.hpp>
#else
#include <kovan/camera.hpp>
#endif
#include <opencv2/highgui/highgui.hpp>

CameraWidget::CameraWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::CameraWidget),
	m_device(new Camera::Device(new CameraInputAdapter(&CameraInputManager::ref()))),
	m_model(new ChannelConfigurationsModel(this))
{
	ui->setupUi(this);
	performStandardSetup(tr("Camera"), false);
	QAction *selectSource = menuBar()->addAction(tr("Source..."));
	connect(selectSource, SIGNAL(triggered()), SLOT(selectSource()));
	
	ui->config->setModel(m_model);
	ui->config->setRootModelIndex(m_model->index(m_model->rootPath()));
	
	ui->config->setCurrentIndex(m_model->defaultConfiguration().row());
	
	connect(ui->config, SIGNAL(currentIndexChanged(int)), SLOT(currentIndexChanged(int)));
	
	m_device->open();
	connect(&CameraInputManager::ref(), SIGNAL(frameAvailable(cv::Mat)),
		SLOT(updateImage()), Qt::QueuedConnection);
}

CameraWidget::~CameraWidget()
{
	m_device->close();
	delete ui;
	delete m_device;
}

void CameraWidget::updateImage()
{
	if(!isVisible()) return;
	if(!m_device->update()) {
		qWarning() << "camera update failed";
		return;
	}
	
	cv::Mat image = m_device->rawImage();
	int h = 0;
	const static int hStep = 137; // Golden angle
	Camera::ChannelPtrVector::const_iterator it = m_device->channels().begin();
	// cv::imshow("Device Image", image);
	for(; it != m_device->channels().end(); ++it, h += hStep) {
		const QColor rectColor = QColor::fromHsv(h % 360, 255, 255);
		const Camera::ObjectVector *objs = (*it)->objects();
		Camera::ObjectVector::const_iterator oit = objs->begin();
		for(; oit != objs->end(); ++oit) {
			const Camera::Object &obj = *oit;
			cv::rectangle(image, cv::Rect(obj.boundingBox().x(), obj.boundingBox().y(),
				obj.boundingBox().width(), obj.boundingBox().height()),
				cv::Scalar(rectColor.red(), rectColor.blue(), rectColor.blue()), 2);
		}
	}
	
	ui->camera->updateImage(image);
}

void CameraWidget::currentIndexChanged(const int &index)
{
	const QString path = m_model->filePath(m_model->index(index,
		0, ui->config->rootModelIndex()));
	Config *conf = Config::load(path.toStdString());
	m_device->setConfig(conf ? *conf : Config());
	delete conf;
}

void CameraWidget::selectSource()
{
	RootController::ref().presentWidget(new CameraInputSelectorWidget(device()));
}
