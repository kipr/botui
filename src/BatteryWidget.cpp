#include "BatteryWidget.h"
#include "BatteryLevelProvider.h"

#include <QPainter>
#include <QTimer>

#define TEXT_SIZE 45

BatteryWidget::BatteryWidget(QWidget *parent)
	: QWidget(parent), m_batteryLevelProvider(0)
{
	QTimer *timer = new QTimer(this);
	timer->start(10000);
	connect(timer, SIGNAL(timeout()), SLOT(update()));
}

BatteryWidget::BatteryWidget(BatteryLevelProvider *batteryLevelProvider, QWidget *parent)
	: QWidget(parent), m_batteryLevelProvider(batteryLevelProvider)
{
	setMinimumSize(25 + TEXT_SIZE, 14);
	setMaximumSize(25 + TEXT_SIZE, 14);
}


void BatteryWidget::setBatteryLevelProvider(BatteryLevelProvider *batteryLevelProvider)
{
	m_batteryLevelProvider = batteryLevelProvider;
}

BatteryLevelProvider *BatteryWidget::batteryLevelProvider() const
{
	return m_batteryLevelProvider;
}

void BatteryWidget::paintEvent(QPaintEvent *event)
{
	const float batteryLevel = m_batteryLevelProvider
		? m_batteryLevelProvider->batteryLevel() : 0.0;
	const float batteryLevelMin = m_batteryLevelProvider
		? m_batteryLevelProvider->batteryLevelMin() : 0.0;
	const float batteryLevelMax = m_batteryLevelProvider
		? m_batteryLevelProvider->batteryLevelMax() : 0.0;
	
	float percentage = m_batteryLevelProvider
		? (batteryLevel - batteryLevelMin) / (batteryLevelMax - batteryLevelMin)
		: 0.0;
	percentage = percentage < 0 ? 0 : percentage;
	percentage = percentage > 1.0 ? 1.0 : percentage;
	
	const int w = width();
	const int h = height();
	
	const static int connectorSize = 3;
	const static int textSize = TEXT_SIZE;
	
	const int adjustedWidth = w - connectorSize - textSize;
	
	QPainter painter(this);
	//painter.setRenderHint(QPainter::Antialiasing);
	QPointF polygon[8] = {
		QPointF(0, 0),
		QPointF(adjustedWidth, 0),
		QPointF(adjustedWidth, h / 4.0),
		QPointF(adjustedWidth + connectorSize, h / 4.0),
		QPointF(adjustedWidth + connectorSize, 3.0 * h / 4.0),
		QPointF(adjustedWidth, 3.0 * h / 4.0),
		QPointF(adjustedWidth, h - 1),
		QPointF(0, h - 1)
	};
	painter.setPen(QColor(30, 30, 30));
	painter.setBrush(Qt::darkGray);
	painter.drawPolygon(polygon, 8);
	
	const int offset = 2;
	painter.setPen(Qt::NoPen);
	
	const bool charging = m_batteryLevelProvider->isCharging();
	
	if(m_batteryLevelProvider && charging)
		painter.setBrush(QColor(0x7D, 0xF9, 0xFF)); // Electric Blue
	else if(percentage < 0.33) painter.setBrush(Qt::red);
	else if(percentage < 0.66) painter.setBrush(Qt::yellow);
	else painter.setBrush(Qt::green);
	
	const float barWidth = adjustedWidth * percentage;
	QRect rect(offset, offset, (barWidth > offset ? barWidth : offset) - offset - 1, h - offset * 2);
	painter.drawRect(rect);
	
	painter.setPen(Qt::black);
	painter.setBrush(Qt::NoBrush);
	
	QRectF textRect(adjustedWidth + connectorSize + offset, 0, textSize - offset, h);
	QTextOption textOpt(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter);
	if(m_batteryLevelProvider) {
		if(!charging) painter.drawText(textRect, QString::number(percentage * 100, 'g', 3) + "\%", textOpt);
		else painter.drawText(textRect, "CHRG", textOpt);
	} else painter.drawText(textRect, "N/A", textOpt);
	
	if(m_batteryLevelProvider && charging) {
		painter.drawLine(3.0 * adjustedWidth / 7.0, 4, 3.0 * adjustedWidth / 7.0, 6);
		painter.drawLine(4.0 * adjustedWidth / 7.0, 4, 4.0 * adjustedWidth / 7.0, 6);
		painter.setBrush(Qt::black);
		painter.drawRect(3.0 * adjustedWidth / 7.0 - offset, 6, adjustedWidth / 7.0 + offset * 2, 3);
		painter.drawRect(3.5 * adjustedWidth / 7.0 - 1, 9, 1, 2);
	}
}

void BatteryWidget::constrain()
{
	setMinimumSize(45, 14);
	setMaximumSize(45, 14);
}