#include "DepthImageView.h"
#include <QPainter>
#include <QDebug>
#include <kovan/depth_image.hpp>

using namespace depth;

DepthImageView::DepthImageView(QWidget *const parent)
  : QWidget(parent)
  , _pleaseWait(true)
{
}

DepthImageView::~DepthImageView()
{
}

void DepthImageView::setDepthImage(const DepthImage *const image)
{
  _image = image;
  _pleaseWait = false;
  if(!_image) {
    _backing = QPixmap();
    return;
  }
  const uint32_t w = _image->width();
  const uint32_t h = _image->height();
  const uint32_t s = w * h;
  
  if(w != _backing.width() || h != _backing.height()) _backing = QPixmap(w, h);
  
  QPainter p;
  if(!p.begin(&_backing)) {
    qDebug() << "Paint to backing failed.";
    return;
  }
  
  for(uint32_t i = 0; i < s; ++i) {
    const uint32_t v = _image->depthAt(i / w, i % w);
    const uint32_t vf = qMin(359U, v >> 7);
    const QColor c = QColor::fromHsv(vf, 250, 250);
    p.setPen(c);
    p.drawPoint(i % w, i / w);
  }
  
  p.end();
  
  repaint();
}

const DepthImage *DepthImageView::depthImage() const
{
  return _image;
}

void DepthImageView::setPleaseWait(const bool pleaseWait)
{
  _pleaseWait = pleaseWait;
}

bool DepthImageView::isPleaseWait() const
{
  return _pleaseWait;
}

void DepthImageView::paintEvent(QPaintEvent *event)
{
  QPainter p(this);
  p.fillRect(0, 0, width(), height(), palette().color(QPalette::Active, QPalette::Window));
  
  if(_pleaseWait) {
    p.drawText(0, 0, width(), height(), Qt::AlignHCenter | Qt::AlignVCenter,
      tr("Just a second..."));
    return;
  }
  if(_backing.isNull()) {
    p.drawText(0, 0, width(), height(), Qt::AlignHCenter | Qt::AlignVCenter,
      tr("No depth sensor detected"));
    return;
  }
  
  p.drawPixmap(0, 0, width(), height(), _backing);
}