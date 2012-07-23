#include "CvWidget.h"

#include <QPainter>

CvWidget::CvWidget(QWidget *parent)
	: QWidget(parent),
	m_invalid(true)
{
}

CvWidget::~CvWidget()
{

}

void CvWidget::setInvalid(bool invalid)
{
	m_invalid = invalid;
}

const bool& CvWidget::invalid() const
{
	return m_invalid;
}

void CvWidget::updateImage(cv::Mat image)
{
	if(m_invalid) return;
	m_image = QImage(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
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
	if(m_invalid) {
		painter.drawText(QRect(0, 0, width(), height()),
			tr("No Camera"),
			QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
		return;
	}
	painter.drawImage(0, height() / 2 - m_resizedImage.height() / 2, m_resizedImage,
		0, 0, m_resizedImage.width(), m_resizedImage.height());
	painter.drawRect(0, height() / 2 - m_resizedImage.height() / 2, m_resizedImage.width(), m_resizedImage.height());
}

void CvWidget::scaleImage()
{
	m_resizedImage = m_image.scaled(size(), Qt::KeepAspectRatio, Qt::FastTransformation);
}