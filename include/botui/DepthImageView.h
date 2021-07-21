#ifndef _DEPTHIMAGE_VIEW_H_
#define _DEPTHIMAGE_VIEW_H_
/* FIXME
#include <QQuickWidget>
#include <QImage>
#include <QPixmap>

namespace depth
{
  class DepthImage;
}

class DepthImageView : public QQuickWidget
{
Q_OBJECT
public:
  DepthImageView(QQuickWidget *const parent = 0);
  ~DepthImageView();
  
  void setDepthImage(const depth::DepthImage *const image);
  const depth::DepthImage *depthImage() const;
  
  void setPleaseWait(const bool pleaseWait);
  bool isPleaseWait() const;
  
protected:
  void paintEvent(QPaintEvent *event);
  
private:
  const depth::DepthImage *_image;
  QImage _backingImage;
  QPixmap _backing;
  bool _pleaseWait;
};
*/
#endif
