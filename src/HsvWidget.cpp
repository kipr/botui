#include "HsvWidget.h"

#include <QPainter>

// Code adapted from Jorge Villatoro's HSV widget for the CBC

#define HUE_OFFSET (300)

HsvWidget::HsvWidget(QWidget *parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_NoSystemBackground, true);
}

inline quint16 HsvWidget::x2sat(const int &x) const
{
	return x * 0xFF / (width() >> 1);
}

inline quint16 HsvWidget::x2val(const int &x) const
{
	const int w = width();
	return 0xFF - (x - w >> 1) * 0xFF / ((w + 1) >> 1);
}

inline quint16 HsvWidget::y2hue(const int &y) const
{
	return (HUE_OFFSET + (y * 360 / height())) % 360;
}

inline int HsvWidget::sat2x(const int &sat) const
{
	return sat * (width() >> 1) / 0xFF;
}

inline int HsvWidget::val2x(const int &val) const
{
	const int w = width();
	return w >> 1 + (0xFF - val) * ((w + 1) >> 1) / 0xFF;
}

inline int HsvWidget::hue2y(const int &hue) const
{
	return (((360 - HUE_OFFSET) + hue) % 360) * height() / 360;
}

void HsvWidget::updateImage()
{
	const int w = width();
	const int h = height();
	//printf("HsvWidget::updateImage(): %d x %d\n", w, h);
	m_image = QImage(w, h, QImage::Format_RGB16);
	if(!w || !h) return;

	for(int y = 0; y < h; ++y) {
		quint16 hue = y2hue(y);
		quint16 *dest = (quint16 *)m_image.scanLine(y);
		for(int x = 0; x < w >> 1; ++x) {
			// *dest++ = cv::Scalar(hue, x2sat(x), 0xFF);
		}
		for(int x = w >> 1; x < w; ++x) {
			// *dest++ = HSV(hue, 0xFF, x2val(x));
		}
	}
}
  
void HsvWidget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.drawImage(QPoint(0, 0), m_image);

	const int winHeight = height();

	// Draw the bounding rectangle for the HSVRange
	// This is a little tricky because the range can wrap in hue (Y)
	
	/* const int left = sat2x(m_range.s.min);
	const int width = val2x(m_range.v.min) - left;
	const int top = hue2y(m_range.h.min);
	
	int height = hue2y(m_range.h.max) - top;
	if(height < 0) height += winHeight;

	painter.setPen(Qt::white);
	painter.drawRect(left, top, width, height);
	painter.drawRect(left, top - winHeight, width, height); */
}

void HsvWidget::resizeEvent(QResizeEvent *)
{
	if (isVisible()) updateImage();
}

void HsvWidget::showEvent(QShowEvent *)
{
	updateImage();
}
