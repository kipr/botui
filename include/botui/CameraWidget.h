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
  
  void setShowBbox(const bool showBbox);
  bool showBbox() const;
  
  void setNumBlobLabels(const int numBlobLabels);
  bool numBlobLabels() const;
  
public slots:
  void update();
  
private:
  void setFrameRate(const unsigned frameRate);
  void slowFrameRate();
  void fastFrameRate();
  
  Camera::Device *m_camDevice;
  QTimer *m_timer;
  bool m_showBbox;
  int m_numBlobLabels;
  unsigned m_frameRate;
};

#endif
