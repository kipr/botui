#include "CameraWidget.h"
#include "ui_CameraWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "ChannelConfigurationsModel.h"
#include <QDebug>
#include <QTimer>
#include <QCoreApplication>

#include <kovan/camera.hpp>

CameraWidget::CameraWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::CameraWidget),
	m_device(new Camera::Device),
	m_timer(new QTimer(this)),
	m_model(new ChannelConfigurationsModel(this))
{
	ui->setupUi(this);
	performStandardSetup(tr("Camera"), false);
	
	ui->config->setModel(m_model);
	qDebug() << m_model->rootPath();
	ui->config->setRootModelIndex(m_model->index(m_model->rootPath()));
	
	ui->config->setCurrentIndex(m_model->defaultConfiguration().row());
	
	connect(ui->config, SIGNAL(currentIndexChanged(int)), SLOT(currentIndexChanged(int)));
	
	connect(m_timer, SIGNAL(timeout()), SLOT(updateCamera()));
	updateCamera();
}

CameraWidget::~CameraWidget()
{
	m_device->close();
	delete ui;
	delete m_device;
}

void CameraWidget::updateCamera()
{
	if(!m_device->isOpen()) {
		if(!m_device->open()) {
			setUpdateSlow();
			return;
		}
		m_device->setWidth(320);
		m_device->setHeight(240);
		ui->camera->setInvalid(false);
		setUpdateFast();
	}
	if(!m_device->update()) {
		qWarning() << "camera update failed";
		ui->camera->setInvalid(true);
		m_device->close();
		setUpdateSlow();
		return;
	}
	
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

void CameraWidget::setUpdateFast()
{
	m_timer->start(75); // 10 FPS
}

void CameraWidget::setUpdateSlow()
{
	m_timer->start(1000);
}
