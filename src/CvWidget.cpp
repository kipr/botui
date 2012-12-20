#include "CvWidget.h"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

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
	cv::Mat dest;
	cv::cvtColor(image, dest,CV_BGR2RGB);
	m_image = QImage(dest.data, dest.cols, dest.rows, dest.step, QImage::Format_RGB888);
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
		painter.fillRect(0, 0, width() - 1, height() - 1, Qt::transparent);
		painter.drawText(QRect(0, 0, width(), height()),
			tr("No Camera"),
			QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
		return;
	}
	painter.drawImage(width() / 2 - m_resizedImage.width() / 2, height() / 2 - m_resizedImage.height() / 2, m_resizedImage,
		0, 0, m_resizedImage.width(), m_resizedImage.height());
	painter.drawRect(width() / 2 - m_resizedImage.width() / 2, height() / 2 - m_resizedImage.height() / 2, m_resizedImage.width() - 1, m_resizedImage.height() - 1);
}

void CvWidget::mousePressEvent(QMouseEvent *event)
{
	if(m_invalid) return;
	const QPointF &pos = event->pos();
	emit pressed(pos.x() / width() * m_image.width(),
		pos.y() / height() * m_image.height());
}

void CvWidget::scaleImage()
{
	m_resizedImage = m_image.scaled(size(), Qt::KeepAspectRatio, Qt::FastTransformation);
}