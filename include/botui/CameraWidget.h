#ifndef _CAMERAWIDGET_H_
#define _CAMERAWIDGET_H_

#include "CvWidget.h"

#include <QTimer>

#ifdef WALLABY
#include <wallaby/config.hpp>
#else
#include <kovan/config.hpp>
#endif

namespace Camera
{
	class Device;
}

class CameraWidget : public CvWidget
{
Q_OBJECT
public:
  CameraWidget(QWidget *parent = 0);
  ~CameraWidget();
  
  void setConfig(Config *config);
  void setChannelConfig(const Config &config, int channelNum);
  
  void setTrackBlobs(const bool trackBlobs);
  bool trackBlobs() const;
  
public slots:
  void update();
  
private:
  Camera::Device *m_camDevice;
  QTimer *m_timer;
  bool m_trackBlobs;
};

#endif