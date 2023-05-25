/* FIXME
#include "DepthImageView.h"
#include <QDebug>
#include <QPainter>

#ifdef WOMBAT
#include <wallaby/depth_image.hpp>
#else
#include <kovan/depth_image.hpp>
#endif

using namespace depth;

static QRgb s_lookupTable[350];
static bool s_tableInited = false;

DepthImageView::DepthImageView(QWidget *const parent)
  : QWidget(parent)
  , _pleaseWait(true)
{
  if(!s_tableInited) {
    for(quint16 i = 0; i < 330; ++i) {
      s_lookupTable[i] = QColor::fromHsv(i, 255, 255).rgb();
    }
    s_lookupTable[0] = 0;
    s_tableInited = true;
  }
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

  if(_backingImage.isNull() || w != _backingImage.width() || h !=
_backingImage.height()) { _backingImage = QImage(w, h, QImage::Format_RGB32);
  }

  for(uint32_t i = 0; i < s; ++i) {
    // 500 is theoretical min value for Xtion, subtract for hsv offset
    const int32_t v = (int32_t)_image->depthAt(i / w, i % w) - 500;
    const int32_t vf = qMax(qMin(330, (v * 330) >> 12), 0);

    _backingImage.setPixel(i % w, i / w, s_lookupTable[vf]);
  }

  _backing = QPixmap::fromImage(_backingImage).scaled(width(), height());

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
  p.fillRect(0, 0, width(), height(), palette().color(QPalette::Active,
QPalette::Window));

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
*/
