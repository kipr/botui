#include "CameraWidget.h"
#include "ui_CameraWidget.h"
#include "Device.h"
#include "ChannelConfigurationsModel.h"
#include "CameraInputManager.h"
#include <QDebug>

#include <kovan/camera.hpp>

CameraWidget::CameraWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::CameraWidget),
	m_device(new Camera::Device(new CameraInputAdapter(&CameraInputManager::ref()))),
	m_model(new ChannelConfigurationsModel(this))
{
	ui->setupUi(this);
	performStandardSetup(tr("Camera"), false);
	
	ui->config->setModel(m_model);
	qDebug() << m_model->rootPath();
	ui->config->setRootModelIndex(m_model->index(m_model->rootPath()));
	
	ui->config->setCurrentIndex(m_model->defaultConfiguration().row());
	
	connect(ui->config, SIGNAL(currentIndexChanged(int)), SLOT(currentIndexChanged(int)));
	
	CameraInputManager::ref().setInputProvider(new Camera::UsbInputProvider);
	connect(&CameraInputManager::ref(), SIGNAL(frameAvailable(cv::Mat)), SLOT(updateImage()));
	m_device->open();
	updateImage();
}

CameraWidget::~CameraWidget()
{
	m_device->close();
	delete ui;
	delete m_device;
}

void CameraWidget::updateImage()
{
	if(!m_device->update()) {
		qWarning() << "camera update failed";
		ui->camera->setInvalid(true);
		return;
	} else ui->camera->setInvalid(false);
	
	int h = 0;
	const static int hStep = 137; // Golden angle
	Camera::ChannelPtrVector::const_iterator it = m_device->channels().begin();
	cv::Mat image = m_device->rawImage();
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
	
	if(image.empty()) qWarning() << "EMPTY!";
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
