#include "CameraWidget.h"

#include <QDebug>
#include <sstream>

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
  connect(m_timer, SIGNAL(timeout()), SLOT(update()));
  this->fastFrameRate();
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

void CameraWidget::setShowBbox(const bool showBbox)
{
  m_showBbox = showBbox;
}

bool CameraWidget::showBbox() const
{
  return m_showBbox;
}

void CameraWidget::setNumBlobLabels(const int numBlobLabels)
{
  m_numBlobLabels = numBlobLabels;
}
bool CameraWidget::numBlobLabels() const
{
  return m_numBlobLabels;
}

// TODO: Make it configurable which channels to show
void CameraWidget::update()
{
  if(!m_camDevice->isOpen()) {
    // Camera isn't open, so try to open
    if(!m_camDevice->open()) {
      // Camera not available; lower frame rate and quit
      this->slowFrameRate();
      return;
    }
    
    // Successfully opened camera; raise frame rate
    this->fastFrameRate();
  }
  
  if(!isVisible()) return;
  
  cv::Mat image;
  if(!m_camDevice->update()) {
    // Update failed, so close camera and lower frame rate
    m_camDevice->close();
    this->slowFrameRate();
  }
  else {
    //qDebug() << "Camera updated!";
    image = m_camDevice->rawImage();
    // If we need to draw additional things...
    if(m_showBbox || m_numBlobLabels > 0) {
      int h = 0;
      const static int hStep = 137; // Golden angle
      
      // Iterate over all channels
      Camera::ChannelPtrVector::const_iterator it = m_camDevice->channels().begin();
      for(; it != m_camDevice->channels().end(); ++it, h += hStep) {
        const QColor rectColor = QColor::fromHsv(h % 360, 255, 255);
        
        // Iterate over all objects for this channel
        const Camera::ObjectVector *objs = (*it)->objects();
        for(int objNum = 0; objNum < objs->size(); ++objNum) {
          const Camera::Object &obj = objs->at(objNum);
          
          // If needed, draw bbox
          if(m_showBbox) {
            cv::rectangle(image, cv::Rect(obj.boundingBox().x(), obj.boundingBox().y(),
              obj.boundingBox().width(), obj.boundingBox().height()),
              cv::Scalar(rectColor.red(), rectColor.blue(), rectColor.blue()), 2);
          }
          
          // If needed, draw blob labels
          if(m_numBlobLabels > objNum) {
            std::ostringstream stm;
            stm << objNum;
            const std::string &numStr = stm.str();
            const int fontFace = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
            const double fontScale = 0.4;
            const int thickness = 1;
            int baseline = 0;
            cv::Size textSize = cv::getTextSize(numStr, fontFace, fontScale, thickness, &baseline);
            baseline += thickness;
            cv::Point textOrg(obj.boundingBox().x() + obj.boundingBox().width()/2 - textSize.width/2,
                              obj.boundingBox().y() + obj.boundingBox().height()/2 + textSize.height/2);
            cv::putText(image, numStr, textOrg, fontFace, fontScale, cv::Scalar::all(255), thickness, 8);
          }
        }
      }
    }
  }
  
  this->updateImage(image);
}

void CameraWidget::setFrameRate(const unsigned frameRate)
{
  if(frameRate == m_frameRate)
    return;
  
  m_frameRate = frameRate;
  if(m_frameRate == 0)
    m_timer->stop();
  else
    m_timer->start(1000.0 / m_frameRate); 
}

void CameraWidget::slowFrameRate()
{
  this->setFrameRate(1);
}

void CameraWidget::fastFrameRate()
{
  this->setFrameRate(10);
}
