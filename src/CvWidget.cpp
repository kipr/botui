#include "CvWidget.h"

#include <QPainter>

CvWidget::CvWidget(QWidget *parent)
	: QWidget(parent)
{
}

CvWidget::~CvWidget()
{

}

void CvWidget::updateImage(IplImage *image)
{
	m_image = QImage((uchar *)image->imageData, image->width, image->height, image->widthStep, QImage::Format_RGB888);
	scaleImage();
	update();
}

void CvWidget::resizeEvent(QResizeEvent *event)
{
	scaleImage();
	QWidget::resizeEvent(event);
}

void CvWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.drawImage(0, height() / 2 - m_resizedImage.height() / 2, m_resizedImage,
		0, 0, m_resizedImage.width(), m_resizedImage.height());
	painter.drawRect(0, height() / 2 - m_resizedImage.height() / 2, m_resizedImage.width(), m_resizedImage.height());
}

void CvWidget::scaleImage()
{
	m_resizedImage = m_image.scaled(size(), Qt::KeepAspectRatio, Qt::FastTransformation);
}