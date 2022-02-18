#include "CvWidget.h"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <opencv2/highgui/highgui.hpp>

CvWidget::CvWidget(QWidget *parent)
	: QWidget(parent),
	m_invalid(true),
	m_data(0)
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

void CvWidget::updateImage(const cv::Mat &image)
{
	m_mutex.lock();
	m_invalid = image.empty();
	if(m_invalid) {
		m_image = cv::Mat();
		update();
		m_mutex.unlock();
		return;
	}
#if CV_VERSION_EPOCH == 3
  cv::cvtColor(image, m_image, cv::COLOR_BGR2RGB);
#else
	cv::cvtColor(image, m_image, cv::COLOR_BGR2RGB);
#endif
	scaleImage();
	update();
	m_mutex.unlock();
}

void CvWidget::resizeEvent(QResizeEvent *event)
{
	scaleImage();
	QWidget::resizeEvent(event);
}

void CvWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	
	if(m_invalid || m_image.empty()) {
		painter.fillRect(0, 0, width() - 1, height() - 1, Qt::transparent);
		painter.drawText(QRect(0, 0, width(), height()),
			tr("No image available. Check camera connection."),
			QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
		return;
	}
	painter.drawImage(width() / 2 - m_resizedImage.width() / 2, height() / 2 - m_resizedImage.height() / 2, m_resizedImage,
		0, 0, m_resizedImage.width(), m_resizedImage.height());
	painter.drawRect(width() / 2 - m_resizedImage.width() / 2, height() / 2 - m_resizedImage.height() / 2,
		m_resizedImage.width() - 1, m_resizedImage.height() - 1);
}

void CvWidget::mousePressEvent(QMouseEvent *event)
{
	if(m_invalid) return;
	const QPointF &pos = event->pos();
	emit pressed(m_image, pos.x() / width() * m_image.cols,
		pos.y() / height() * m_image.rows);
}

void CvWidget::scaleImage()
{
	if(m_invalid || m_image.empty()) {
		qDebug() << "Can't scale an empty image";
		return;
	}
	
	cv::Mat resized;
	cv::resize(m_image, resized, cv::Size(width(), height()));
	delete m_data;
	m_data = new unsigned char[resized.rows * resized.cols * resized.elemSize()];
	memcpy(m_data, resized.ptr(), resized.rows * resized.cols * resized.elemSize());
	m_resizedImage = QImage(m_data, resized.cols, resized.rows, resized.step, QImage::Format_RGB888);
}
