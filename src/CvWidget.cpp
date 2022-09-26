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

void CvWidget::updateImage(const kipr::camera::Image &image)
{
	m_mutex.lock();
	m_invalid = image.isEmpty();

	if(m_invalid) {
		m_image = kipr::camera::Image();
	} else {
		m_image = image;
		this->prepareImageForPainting();
	}

	update();
	m_mutex.unlock();
}

void CvWidget::resizeEvent(QResizeEvent *event)
{
	if (!m_invalid) this->prepareImageForPainting();

	QWidget::resizeEvent(event);
}

void CvWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	
	if(m_invalid || m_resizedImage.isNull()) {
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
	emit pressed(m_image, pos.x() / width() * m_image.getWidth(),
		pos.y() / height() * m_image.getHeight());
}

void CvWidget::postProcessImage(QImage &image)
{
	// No-op for CvWidget, but subclasses can override it to augment the displayed image
}

void CvWidget::prepareImageForPainting()
{
	QImage::Format format;
	switch (m_image.getType()) {
	case kipr::camera::Image::Type::Rgb888:
		format = QImage::Format_RGB888;
		break;
	case kipr::camera::Image::Type::Bgr888:
		format = QImage::Format_BGR888;
		break;
	case kipr::camera::Image::Type::Grey8:
		format = QImage::Format_Grayscale8;
		break;
	default:
		qDebug() << "Unsupported image format";
		m_resizedImage = QImage();
		return;
	}

	QImage originalImage(m_image.getData(), m_image.getWidth(), m_image.getHeight(), m_image.getStride(), format);
	this->postProcessImage(originalImage);

	m_resizedImage = originalImage.scaled(width(), height(), Qt::KeepAspectRatio);
}
