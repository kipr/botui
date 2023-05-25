#ifndef _CAMERAINPUTMANAGER_H_
#define _CAMERAINPUTMANAGER_H_

#include "Singleton.h"
#include <QObject>
#include <QTimer>

#include <kipr/camera/camera.hpp>

class CameraInputManager : public QObject,
                           public Singleton<CameraInputManager> {
    Q_OBJECT
  public:
    enum Source { Nil = 0, UsbCamera, ARDroneFront, ARDroneBottom };

    CameraInputManager();

    void setSource(const Source source);
    Source source() const;

    void setFrameRate(int frameRate);
    int frameRate() const;

    bool retain();
    bool release();

    bool isOpen() const;

    kipr::camera::Image image() const;

  signals:
    void frameAvailable(const kipr::camera::Image &image);

  public slots:
    void updateCamera();

  private:
    kipr::camera::Device *m_camDevice;
    Source m_source;
    QTimer *m_timer;
    kipr::camera::Image m_image;
    int m_frameRate;
    int m_refs;
    bool m_reentryBarrier;
};

/*class CameraInputAdapter : public Camera::InputProvider
{
public:
    CameraInputAdapter(CameraInputManager *manager);
    ~CameraInputAdapter();

    virtual bool open(const int number);
    virtual bool isOpen() const;
    virtual void setWidth(const unsigned width);
    virtual void setHeight(const unsigned height);
    virtual bool next(cv::Mat &image);
    virtual bool close();

private:
    CameraInputManager *m_manager;
};*/

#endif
