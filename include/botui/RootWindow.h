#ifndef ROOTWINDOW_H
#define ROOTWINDOW_H

#include <QWidget>

class QStackedWidget;
class QResizeEvent;

class RootWindow : public QWidget
{
Q_OBJECT
public:
	explicit RootWindow(QWidget *parent = nullptr);

	QStackedWidget *pageStack() const;
	QWidget *overlayLayer() const;

	void showOverlay(QWidget *widget);
	void hideOverlay(QWidget *widget);

protected:
	void resizeEvent(QResizeEvent *event) override;

private:
	QStackedWidget *m_pageStack;
	QWidget *m_overlayLayer;
	QWidget *m_overlayWidget;
};

#endif // ROOTWINDOW_H
