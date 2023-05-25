#include "CameraInputManager.h"

#include "ConnectionedARDroneInputProvider.h"
#include <QDebug>

CameraInputManager::CameraInputManager()
    : m_camDevice(new kipr::camera::Device()),
      m_source(CameraInputManager::UsbCamera),
      m_timer(new QTimer(this)),
      m_frameRate(1),
      m_refs(0),
      m_reentryBarrier(false) {
    qRegisterMetaType<kipr::camera::Image>("kipr::camera::Image");
    m_image = kipr::camera::Image();
    connect(m_timer, SIGNAL(timeout()), SLOT(updateCamera()));
}

void CameraInputManager::setSource(const CameraInputManager::Source source) {
    m_source = source;

    qWarning() << "WARNING: CameraInputManager::setSource not implemented";
}

CameraInputManager::Source CameraInputManager::source() const {
    return m_source;
}

void CameraInputManager::setFrameRate(int frameRate) {
    m_frameRate = frameRate;
    m_timer->start(1000.0 / m_frameRate);
}

int CameraInputManager::frameRate() const { return m_frameRate; }

bool CameraInputManager::retain() {
    ++m_refs;
    updateCamera();

    return true;
}

bool CameraInputManager::release() {
    qWarning() << "Releasing!";
    --m_refs;
    if (m_refs < 0) {
        qWarning() << "Release called when reference count was 0!";
        m_refs = 0;
        return false;
    }
    if (m_refs == 0) {
        qWarning() << "Closing!";
        bool success = m_camDevice->close();
        qWarning() << success;
        return success;
    }

    return true;
}

bool CameraInputManager::isOpen() const { return m_camDevice->isOpen(); }

kipr::camera::Image CameraInputManager::image() const { return m_image; }

void CameraInputManager::updateCamera() {
    if (m_reentryBarrier) return;
    m_reentryBarrier = true;

    if (!m_refs || !m_camDevice) {
        setFrameRate(1);
        m_reentryBarrier = false;
        return;
    }
    if (!m_camDevice->isOpen()) {
        qWarning() << "Camera device is not yet open";
        if (!m_camDevice->open()) {
            qDebug() << "Failed to open camera device... for now.";
            setFrameRate(1);
            m_reentryBarrier = false;
            return;
        }
        // m_inputProvider->setWidth(320);
        // m_inputProvider->setHeight(240);
        setFrameRate(20);
    }

    if (!m_camDevice->update()) {
        qWarning() << "Camera update failed";
        // ui->camera->setInvalid(true);
        m_camDevice->close();
        setFrameRate(1);
        m_image = kipr::camera::Image();
    }
    else
        m_image = m_camDevice->rawImage();

    if (!m_image.isEmpty()) emit frameAvailable(m_image);
    m_reentryBarrier = false;
}

/*void CameraInputManager::setInputProvider(Camera::InputProvider
*inputProvider)
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
}*/

/*CameraInputAdapter::CameraInputAdapter(CameraInputManager *manager)
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
}*/
