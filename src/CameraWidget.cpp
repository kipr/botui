#include "CameraWidget.h"

#include <QDebug>

#ifdef WALLABY
#include <wallaby/camera.hpp>
#else
#include <kovan/camera.hpp>
#endif

CameraWidget::CameraWidget(QWidget *parent)
  : CvWidget(parent),
  m_camDevice(new Camera::Device()),
  m_timer(new QTimer(this))
{
  m_camDevice->open();
  // TODO: Smarter fps system
  connect(m_timer, SIGNAL(timeout()), SLOT(update()));
  m_timer->start(130);
}

CameraWidget::~CameraWidget()
{
  m_camDevice->close();
  delete m_timer;
  delete m_camDevice;
}

void CameraWidget::setConfig(Config *config)
{
  m_camDevice->setConfig(config ? *config : Config());
}

void CameraWidget::setChannelConfig(const Config &config, int channelNum)
{
  if(m_camDevice->channels().size() <= channelNum)
    return;
  
  m_camDevice->channels()[channelNum]->setConfig(config);
}

void CameraWidget::setTrackBlobs(const bool trackBlobs)
{
  m_trackBlobs = trackBlobs;
}

bool CameraWidget::trackBlobs() const
{
  return m_trackBlobs;
}

// TODO: Make it configurable which channels to show
void CameraWidget::update()
{
  // TODO: Gracefully handle camera connections (retry, lower fps, etc.)
  if(!isVisible()) return;
  if(!m_camDevice->update()) {
    qWarning() << "camera update failed";
    return;
  }
  qWarning() << "camera updated";
  
  cv::Mat image = m_camDevice->rawImage();
  if(m_trackBlobs) {
    int h = 0;
    const static int hStep = 137; // Golden angle
    Camera::ChannelPtrVector::const_iterator it = m_camDevice->channels().begin();
    for(; it != m_camDevice->channels().end(); ++it, h += hStep) {
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
  }
  
  this->updateImage(image);
}