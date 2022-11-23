#include "RootController.h"

#include "BuildOptions.h"
#include "DeclarativeView.h"

#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QApplication>

RootController::RootController()
	: m_dismissable(true)
{
}

void RootController::presentQml(const QUrl &url)
{
	DeclarativeView *view = new DeclarativeView(url);
	view->setAutoReload(DEVELOPER_MODE);
	presentWidget(view, true);
	view->setResizeMode(QQuickWidget::SizeRootObjectToView);
	view->engine()->rootContext()->setContextProperty("rootController", this);
}

int RootController::presentDialog(QDialog *dialog)
{
	setDismissable(false);
	// presentWidget(dialog, false);
	const int ret = dialog->exec();
	setDismissable(true);
	// dismissWidget();
	return ret;
}

void RootController::findNetworkSettingsWidget() const
{
}

void RootController::addNetworkSettingsStack(NetworkSettingsWidget *networkWidget)
{
	m_stack.push(networkWidget);
}

void RootController::getNetworkSettingsWidget()
{
	QWidget *prev = m_stack.size() ? m_stack.top() : 0;
	printStack();
	foreach (QWidget *widge, m_stack)
	{

		if (widge->objectName() == "NetworkSettingsWidget")
		{
			constrain(widge);
			widge->move(prev->pos());
			present(widge);
		}
	}
}
void RootController::printStack()
{
	foreach (QWidget *widge, m_stack)
	{
		qDebug() << widge << " Name: " << widge->objectName();
	}
}
void RootController::presentWidget(QWidget *widget, bool owns)
{
	for (int i = 0; i < m_stack.size(); ++i)
	{
		if (m_stack.at(i) == widget)
			m_stack.remove(i);
	}
	m_ownership[widget] = owns;
	QWidget *prev = m_stack.size() ? m_stack.top() : 0;
	m_stack.push(widget);
	constrain(widget); // constrain size of widget
	if (prev)
		widget->move(prev->pos());
	present(widget);
	if (prev)
		prev->hide();
}

void RootController::dismissWidget()
{
	if (!m_dismissable)
		return;
	// QWidget *widget = m_stack.pop();

	QWidget *widget = m_stack.top(); // reference to top of stack
	
	if (widget->objectName() == "NetworkSettingsWidget")
	{
		widget->hide();
	}
	else
	{
		widget = m_stack.pop();
		qDebug() << "Trying to dismiss Widget: " << widget->objectName();
		QWidget *next = m_stack.size() ? m_stack.top() : 0;
		qDebug() << "Next Widget is:" << next->objectName();
		if (next)
			next->move(widget->pos());

		if(next->objectName() != "NetworkSettingsWidget")
		{
			present(next);
		}

		widget->hide();
		if (m_ownership.value(widget))
			widget->deleteLater();
		m_ownership.remove(widget);
	}
}

void RootController::dismissAllWidgets()
{
	if (!m_dismissable)
		return;
	while (m_stack.size() > 2) //Remaining: HomeWidget and NetworkSettingsWidget
		dismissWidget();
	printStack();
	present(m_stack.at(0));
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

void RootController::constrain(QWidget *widget)
{
	widget->setMinimumSize(800, 480);
	widget->setMaximumSize(800, 480);
}

void RootController::present(QWidget *widget)
{
	if (!widget)
		return;

	if (m_fullscreen)
		widget->showFullScreen();
	else
		widget->show();

	widget->raise();
}
