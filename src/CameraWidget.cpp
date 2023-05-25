#include "CameraWidget.h"

#include <QDebug>
#include <QPainter>
#include <sstream>

#include <kipr/camera/camera.hpp>
#include <kipr/camera/channel.hpp>
#include <kipr/camera/object.hpp>

CameraWidget::CameraWidget(QWidget *parent)
    : CameraImageWidget(parent),
      m_camDevice(new kipr::camera::Device()),
      m_timer(new QTimer(this)) {
    connect(m_timer, SIGNAL(timeout()), SLOT(update()));
    this->fastFrameRate();
}

CameraWidget::~CameraWidget() {
    m_camDevice->close();
    delete m_timer;
    delete m_camDevice;
}

void CameraWidget::setConfig(Config *config) {
    m_camDevice->setConfig(config ? *config : Config());
}

void CameraWidget::setChannelConfig(const Config &config, int channelNum) {
    if (m_camDevice->channels().size() <= channelNum) return;

    m_camDevice->channels()[channelNum]->setConfig(config);
}

void CameraWidget::setShowBbox(const bool showBbox) { m_showBbox = showBbox; }

bool CameraWidget::showBbox() const { return m_showBbox; }

void CameraWidget::setNumBlobLabels(const int numBlobLabels) {
    m_numBlobLabels = numBlobLabels;
}
bool CameraWidget::numBlobLabels() const { return m_numBlobLabels; }

// TODO: Make it configurable which channels to show
void CameraWidget::update() {
    if (!m_camDevice->isOpen()) {
        // Camera isn't open, so try to open
        if (!m_camDevice->open(0, LOW_RES, BLACK_2017)) {
            // Camera not available; lower frame rate and quit
            this->slowFrameRate();
            return;
        }
        this->setInvalid(false);
        // Successfully opened camera; raise frame rate
        this->fastFrameRate();
    }

    if (!isVisible()) return;

    kipr::camera::Image image;
    if (!m_camDevice->update()) {
        // Update failed, so close camera and lower frame rate
        m_camDevice->close();
        this->slowFrameRate();
    }
    else {
        qDebug() << "Camera updated!";
        image = m_camDevice->rawImage();
    }

    this->updateImage(image);
}

void CameraWidget::postProcessImage(QImage &image) {
    if (image.isNull()) return;

    // If there's nothing additional to draw...
    if (!m_showBbox && m_numBlobLabels <= 0) return;

    QPainter painter(&image);

    int h = 0;
    const static int hStep = 137; // Golden angle

    // Iterate over all channels
    std::vector<kipr::camera::Channel *>::const_iterator it =
        m_camDevice->channels().begin();
    for (; it != m_camDevice->channels().end(); ++it, h += hStep) {
        const QColor rectColor = QColor::fromHsv(h % 360, 255, 255);
        QPen pen(rectColor, 2);
        painter.setPen(pen);

        // Iterate over all objects for this channel
        const kipr::camera::ObjectVector *objs = (*it)->objects();
        for (int objNum = 0; objNum < objs->size(); ++objNum) {
            const kipr::camera::Object &obj = objs->at(objNum);

            // If needed, draw bbox
            if (m_showBbox) {
                painter.drawRect(obj.boundingBox().x(),
                                 obj.boundingBox().y(),
                                 obj.boundingBox().width(),
                                 obj.boundingBox().height());
            }

            // If needed, draw blob labels
            if (m_numBlobLabels > objNum) {
                const qreal textRectSize = 32767.0;
                QRectF textRect(
                    obj.boundingBox().x() + obj.boundingBox().width() / 2 -
                        textRectSize / 2.0,
                    obj.boundingBox().y() + obj.boundingBox().height() / 2 -
                        textRectSize + textRectSize / 2.0,
                    textRectSize,
                    textRectSize);

                painter.drawText(textRect,
                                 Qt::AlignHCenter | Qt::AlignVCenter,
                                 QString::number(objNum));
            }
        }
    }
}

void CameraWidget::setFrameRate(const unsigned frameRate) {
    if (frameRate == m_frameRate) return;

    m_frameRate = frameRate;
    if (m_frameRate == 0)
        m_timer->stop();
    else
        m_timer->start(1000.0 / m_frameRate);
}

void CameraWidget::slowFrameRate() { this->setFrameRate(1); }

void CameraWidget::fastFrameRate() { this->setFrameRate(60); }