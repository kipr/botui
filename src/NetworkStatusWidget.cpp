#include "NetworkStatusWidget.h"

#include "NetworkingProvider.h"

#include <QTime>
#include <QPainter>
#include <QDebug>

NetworkStatusWidget::NetworkStatusWidget(QWidget *parent)
	: QWidget(parent), m_networkingProvider(0)
{
	constrain();
}

NetworkStatusWidget::NetworkStatusWidget(NetworkingProvider *networkingProvider, QWidget *parent)
	: QWidget(parent), m_networkingProvider(networkingProvider)
{
	constrain();
}

void NetworkStatusWidget::setNetworkingProvider(NetworkingProvider *networkingProvider)
{
	if(m_networkingProvider) m_networkingProvider->disconnect(this);
	m_networkingProvider = networkingProvider;
	if(m_networkingProvider) connect(m_networkingProvider,
		SIGNAL(networkStateChanged(NetworkState)), SLOT(networkStateChanged()));
}

NetworkingProvider *NetworkStatusWidget::networkingProvider() const
{
	return m_networkingProvider;
}

void NetworkStatusWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	const int w = width();
	const int h = height();
	const int ellipseSize = w / 6;
	const int startAngle = 30 * 16;
	const int spanAngle = 120 * 16;
	static const QColor green = QColor(50, 150, 50);
	static const QColor red = QColor(250, 100, 100);
	
	const bool off = !m_networkingProvider || m_networkingProvider->networkState() == NetworkingProvider::NetworkOff;
	const QColor color = off ? red : green;
	
	painter.setPen(QPen(color, w / 10));
	painter.setBrush(color);
	
	if(off) {
		const int min = w > h ? h : w;
		const float minOff = min / 3.0;
		painter.drawLine(w / 2.0 - minOff, h / 2.0 - minOff,
			w / 2.0 + minOff, h / 2.0 + minOff);
		painter.drawLine(w / 2.0 - minOff, h / 2.0 + minOff,
			w / 2.0 + minOff, h / 2.0 - minOff);
		return;
	}
	
	const float networkStrength = 0.0;
	const float networkStrengthMin = m_networkingProvider
		? m_networkingProvider->networkStrengthMin() : 0.0;
	const float networkStrengthMax = m_networkingProvider
		? m_networkingProvider->networkStrengthMax() : 0.0;
	
	const float percentage = m_networkingProvider
		? (networkStrength - networkStrengthMin) / (networkStrengthMax - networkStrengthMin)
		: 0.0;
	
	QRectF rectangle(0, h / 5.0, w, h);
	if(percentage > 0.66) {
		painter.drawArc(rectangle, startAngle, spanAngle);
	}
	rectangle = QRectF(w / 5.0, 2.0 * h / 5.0, 3.0 * w / 5.0, h / 1.5);
	if(percentage > 0.33) {
		painter.drawArc(rectangle, startAngle, spanAngle);
	}
	painter.setPen(Qt::NoPen);
	
	painter.drawEllipse(w / 2.0 - ellipseSize / 2.0, 3.5 * h / 5.0 - ellipseSize / 2.0,
		ellipseSize, ellipseSize);

}

void NetworkStatusWidget::networkStateChanged()
{
	update();
}

void NetworkStatusWidget::constrain()
{
	setMinimumSize(24, 20);
	setMaximumSize(24, 20);
}