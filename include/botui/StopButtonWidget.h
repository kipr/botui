#ifndef _STOPBUTTONWIDGET_H_
#define _STOPBUTTONWIDGET_H_

#include <QQuickWidget>

class Program;

class StopButtonWidget : public QQuickWidget
{
Q_OBJECT
public:
	StopButtonWidget(Program *program, QQuickWidget *parent = 0);
	virtual QSize sizeHint() const;
signals:
	void clicked();
	
protected:
	void paintEvent(QPaintEvent *event);
	
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
private slots:
	void started();
	void finished();
	
private:
	Program *m_program;
	bool m_running;
	bool m_pressed;
};

#endif
