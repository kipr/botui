#include "RootController.h"

#include "BuildOptions.h"
#include "DeclarativeView.h"

#include <iostream>

#include <QDebug>
#include <QTimer>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QApplication>

#define ORIENTATION_TICK_MS 5

RootController::RootController()
	: m_dismissable(true)
	, m_orientation(Orientation::Normal)
	, m_orientationTimer(nullptr)
{
}

void RootController::presentQml(const QUrl& url)
{
	DeclarativeView *view = new DeclarativeView(url);
	view->setAutoReload(DEVELOPER_MODE);
	presentWidget(view, true);
	view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
	view->engine()->rootContext()->setContextProperty("rootController", this);
}

int RootController::presentDialog(QDialog *dialog)
{
	setDismissable(false);
	//presentWidget(dialog, false);
	const int ret = dialog->exec();
	setDismissable(true);
	//dismissWidget();
	return ret;
}

void RootController::presentWidget(QWidget *widget, bool owns)
{
	QGraphicsScene *scene = new QGraphicsScene(this);
	QGraphicsProxyWidget *w = scene->addWidget(widget);
	w->setTransformOriginPoint(400, 240);

	w->setRotation(0);

	setOrientation_(m_orientation == Orientation::Normal ? 0 : 180);

	QGraphicsView *view = new QGraphicsView();
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	view->setScene(scene);
	
	for(int i = 0; i < m_stack.size(); ++i) {
		if(m_stack.at(i) == widget) m_stack.remove(i);
	}
	m_ownership[widget] = owns;
	QWidget *prev = m_stack.size() ? m_stack.top() : 0;
	m_stack.push(view);
	constrain(view);
	if(prev) view->move(prev->pos());
	present(view);
	if(prev) prev->hide();
}

void RootController::dismissWidget()
{
	if(!m_dismissable) return;
	QWidget *widget = m_stack.pop();
	QWidget *next = m_stack.size() ? m_stack.top() : 0;
	if(next) next->move(widget->pos());
	
	present(next);
	
	widget->hide();
	if(m_ownership.value(widget)) widget->deleteLater();
	m_ownership.remove(widget);
}

void RootController::dismissAllWidgets()
{
	if(!m_dismissable) return;
	while(m_stack.size() > 1) dismissWidget();
}

void RootController::minimize()
{
  m_stack.top()->showMinimized();
}

const unsigned int RootController::depth() const
{
	return m_stack.size();
}

void RootController::setDismissable(bool dismissable)
{
	m_dismissable = dismissable;
}

bool RootController::isDismissable() const
{
	return m_dismissable;
}

void RootController::setFullscreen(const bool fullscreen)
{
  m_fullscreen = fullscreen;
}

bool RootController::isFullscreen() const
{
  return m_fullscreen;
}

void RootController::setOrientation(const Orientation orientation)
{
	
	m_orientation = orientation;

	if(!!m_orientationTimer) return;

	m_orientationTimer = new QTimer();
	m_orientationTimer->start(ORIENTATION_TICK_MS);
	
	connect(m_orientationTimer, SIGNAL(timeout()), this, SLOT(orient()));
}

void RootController::setOrientation_(double rot)
{
	for(QStack<QWidget *>::const_iterator it = m_stack.begin(); it != m_stack.end(); ++it)
	{
		QGraphicsItem *const item = qobject_cast<QGraphicsView *>(*it)->scene()->items()[0];
		item->setRotation(rot);
	}
}

RootController::Orientation RootController::orientation() const
{
	return m_orientation;
}

void RootController::orient()
{
	
	QGraphicsItem *const item = qobject_cast<QGraphicsView *>(m_stack.top())->scene()->items()[0];
	const double rot = item->rotation();

	if(m_orientation == Orientation::Normal)
	{
		if(abs(rot) <= 10)
		{
			setOrientation_(0);
			m_orientationTimer->deleteLater();
			m_orientationTimer = nullptr;
		}
		else
		{
			setOrientation_(rot - 10);
			m_orientationTimer->start(ORIENTATION_TICK_MS);
		}
	}
	else
	{
		if(abs(rot - 180) <= 10)
		{
			item->setRotation(180);
			m_orientationTimer->deleteLater();
			m_orientationTimer = nullptr;
		}
		else
		{
			item->setRotation(rot + 10);
			m_orientationTimer->start(ORIENTATION_TICK_MS);
		}
	}
}

void RootController::constrain(QWidget *widget)
{
	widget->setMinimumSize(800, 480);
	widget->setMaximumSize(800, 480);
}

void RootController::present(QWidget *widget)
{
	if(!widget) return;

  if(m_fullscreen)
	  widget->showFullScreen();
  else
    widget->show();
  
	widget->raise();
}
