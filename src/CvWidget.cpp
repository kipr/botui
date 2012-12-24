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
	cv::cvtColor(image, m_image, CV_BGR2RGB);
	
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
	emit pressed(pos.x() / width() * m_image.cols,
		pos.y() / height() * m_image.rows);
}

void CvWidget::scaleImage()
{
	cv::Mat resized;
	cv::resize(m_image, resized, cv::Size(width(), height()));
	m_resizedImage = QImage(resized.data, resized.cols, resized.rows, resized.step, QImage::Format_RGB888);
}