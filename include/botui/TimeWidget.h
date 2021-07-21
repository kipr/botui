#ifndef _TIMEWIDGET_H_
#define _TIMEWIDGET_H_

#include <QLabel>
#include <QQuickWidget>

class TimeWidget : public QLabel
{
Q_OBJECT
public:
	TimeWidget(QQuickWidget *parent = 0);
private slots:
	void updateTime();
};

#endif
