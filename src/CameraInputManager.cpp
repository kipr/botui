#include "CameraInputManager.h"
#include "ConnectionedARDroneInputProvider.h"
#include <QDebug>

CameraInputManager::CameraInputManager()
	: m_inputProvider(new Camera::UsbInputProvider()),
	m_source(CameraInputManager::UsbCamera),
	m_timer(new QTimer(this)),
	m_frameRate(1),
	m_refs(0),
	m_reentryBarrier(false)
{
	m_image = cv::Mat();
	connect(m_timer, SIGNAL(timeout()), SLOT(updateCamera()));
}

void CameraInputManager::setSource(const CameraInputManager::Source source)
{
	m_source = source;
	
	Camera::InputProvider *inputProvider = 0;
	switch(m_source) {
	case CameraInputManager::UsbCamera:
		inputProvider = new Camera::UsbInputProvider;
		break;
	case CameraInputManager::ARDroneFront:
		inputProvider = new Camera::ConnectionedARDroneInputProvider;
		break;
	default: break;
	}
	
	setInputProvider(inputProvider);
}

CameraInputManager::Source CameraInputManager::source() const
{
	return m_source;
}
	
void CameraInputManager::setFrameRate(int frameRate)
{
	m_frameRate = frameRate;
	m_timer->start(1000.0 / m_frameRate);
}

int CameraInputManager::frameRate() const
{
	return m_frameRate;
}

void CameraInputManager::setWidth(const unsigned width)
{
	m_inputProvider->setWidth(width);
}

void CameraInputManager::setHeight(const unsigned height)
{
	m_inputProvider->setHeight(height);
}
	
bool CameraInputManager::retain()
{
	++m_refs;
	updateCamera();
		
	return true;
}

bool CameraInputManager::release()
{
	qWarning() << "Releasing!";
	--m_refs;
	if(m_refs < 0) {
		qWarning() << "Release called when reference count was 0!";
		m_refs = 0;
		return false;
	}
	if(m_refs == 0) {
		qWarning() << "Closing!";
		bool success = m_inputProvider->close();
		qWarning() << success;
		return success;
	}
		
	return true;
}

bool CameraInputManager::isOpen() const
{
	return m_inputProvider->isOpen();
}

cv::Mat CameraInputManager::image() const
{
	return m_image;
}

void CameraInputManager::updateCamera()
{
	if(m_reentryBarrier) return;
	m_reentryBarrier = true;
	
	if(!m_refs || !m_inputProvider) {
		setFrameRate(1);
		m_reentryBarrier = false;
		return;
	}
	if(!m_inputProvider->isOpen()) {
		if(!m_inputProvider->open(0)) {
			qDebug() << "Failed to open input provider... for now.";
			setFrameRate(1);
			m_reentryBarrier = false;
			return;
		}
		// m_inputProvider->setWidth(320);
		// m_inputProvider->setHeight(240);
		setFrameRate(20);
	}
	
	if(!m_inputProvider->next(m_image)) {
		qWarning() << "Camera update failed";
		// ui->camera->setInvalid(true);
		m_inputProvider->close();
		setFrameRate(1);
		m_image = cv::Mat();
	}
	
	emit frameAvailable(m_image);
	m_reentryBarrier = false;
}

void CameraInputManager::setInputProvider(Camera::InputProvider *inputProvider)
{
	const bool wasOpened = m_inputProvider ? m_inputProvider->isOpen() : false;
	if(m_inputProvider) m_inputProvider->close();
	delete m_inputProvider;
	
	m_inputProvider = inputProvider;
	
	if(m_inputProvider && wasOpened) m_inputProvider->open(0);
}

Camera::InputProvider *CameraInputManager::inputProvider() const
{
	return m_inputProvider;
}

CameraInputAdapter::CameraInputAdapter(CameraInputManager *manager)
	: m_manager(manager)
{
}

CameraInputAdapter::~CameraInputAdapter()
{
}

		
bool CameraInputAdapter::open(const int number)
{
	m_manager->retain();
	
	return true;
}

bool CameraInputAdapter::isOpen() const
{
	return m_manager->isOpen();
}

void CameraInputAdapter::setWidth(const unsigned width)
{
	m_manager->setWidth(width);
}

void CameraInputAdapter::setHeight(const unsigned height)
{
	m_manager->setHeight(height);
}

bool CameraInputAdapter::next(cv::Mat &image)
{
	image = m_manager->image();
	return true;
}

bool CameraInputAdapter::close()
{
	return m_manager->release();
}
