#ifndef _CAMERAIMAGEWIDGET_H_
#define _CAMERAIMAGEWIDGET_H_

#include <QImage>
#include <QMutex>
#include <QWidget>

#include <kipr/camera/camera.hpp>

class CameraImageWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(bool invalid READ invalid WRITE setInvalid)
  public:
    CameraImageWidget(QWidget *parent = 0);
    ~CameraImageWidget();

    void setInvalid(bool invalid);
    const bool &invalid() const;

  public slots:
    void updateImage(const kipr::camera::Image &image);

  signals:
    void pressed(const kipr::camera::Image &image, const int &x, const int &y);

  protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void postProcessImage(QImage &image);

  private:
    void prepareImageForPainting();

    bool m_invalid;
    kipr::camera::Image m_image;
    QImage m_resizedImage;
    mutable QMutex m_mutex;
};

#endif
