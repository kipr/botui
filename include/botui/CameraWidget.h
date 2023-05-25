#ifndef _CAMERAWIDGET_H_
#define _CAMERAWIDGET_H_

#include "CameraImageWidget.h"

#include <QTimer>

#include <kipr/config/config.hpp>

using namespace kipr::config;

namespace kipr::camera {
class Device;
}

class CameraWidget : public CameraImageWidget {
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

  protected:
    void postProcessImage(QImage &image) override;

  private:
    void setFrameRate(const unsigned frameRate);
    void slowFrameRate();
    void fastFrameRate();

    kipr::camera::Device *m_camDevice;
    QTimer *m_timer;
    bool m_showBbox;
    int m_numBlobLabels;
    unsigned m_frameRate;
};

#endif