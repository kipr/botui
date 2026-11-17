#include "RootController.h"

#include "BuildOptions.h"
#include "DeclarativeView.h"
#include "RootWindow.h"

#include <QDebug>
#include <QDialog>
#include <QEventLoop>
#include <QMetaObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QSizePolicy>
#include <QStackedWidget>

namespace {
void preparePage(QWidget *widget)
{
	// Page layouts may have size hints larger than the display. They must fill
	// the persistent shell without changing its Wayland surface dimensions.
	widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}
}

RootController::RootController()
	: m_window(nullptr),
	  m_activeInput(nullptr),
	  m_dismissable(true)
{
}

void RootController::initialize(RootWindow *window, QWidget *rootPage)
{
	if (!window || !rootPage) {
		qCritical() << "RootController requires a window and root page";
		return;
	}
	if (m_window || !m_stack.isEmpty()) {
		qCritical() << "RootController has already been initialized";
		return;
	}

	m_window = window;
	preparePage(rootPage);
	m_window->pageStack()->addWidget(rootPage);
	m_stack.push(rootPage);
	m_window->pageStack()->setCurrentWidget(rootPage);
}

void RootController::presentQml(const QUrl &url)
{
	DeclarativeView *view = new DeclarativeView(url);
	view->setAutoReload(DEVELOPER_MODE);
	view->setResizeMode(QQuickWidget::SizeRootObjectToView);
	view->engine()->rootContext()->setContextProperty("rootController", this);
	presentWidget(view);
}

int RootController::presentDialog(QDialog *dialog)
{
	if (!dialog)
		return QDialog::Rejected;
	if (m_activeInput) {
		qWarning() << "Cannot present a dialog while an input overlay is active";
		return QDialog::Rejected;
	}

	if (!dialog->parentWidget() && currentWidget())
		dialog->setParent(currentWidget(), Qt::Dialog);

	const bool wasDismissable = m_dismissable;
	setDismissable(false);
	const int result = dialog->exec();
	setDismissable(wasDismissable);
	return result;
}

void RootController::presentWidget(QWidget *widget)
{
	if (!navigationAllowed(widget))
		return;

	preparePage(widget);
	m_window->pageStack()->addWidget(widget);
	m_stack.push(widget);
	m_window->pageStack()->setCurrentWidget(widget);
}

void RootController::replaceWidget(QWidget *widget)
{
	if (!navigationAllowed(widget))
		return;
	if (m_stack.isEmpty()) {
		presentWidget(widget);
		return;
	}

	QWidget *oldWidget = m_stack.pop();
	preparePage(widget);
	m_window->pageStack()->addWidget(widget);
	m_stack.push(widget);
	m_window->pageStack()->setCurrentWidget(widget);
	m_window->pageStack()->removeWidget(oldWidget);
	oldWidget->deleteLater();
}

void RootController::dismissWidget()
{
	if (!m_dismissable || m_activeInput || m_stack.size() <= 1)
		return;

	QWidget *widget = m_stack.pop();
	m_window->pageStack()->setCurrentWidget(m_stack.top());
	m_window->pageStack()->removeWidget(widget);
	widget->deleteLater();
}

void RootController::dismissAllWidgets()
{
	if (!m_dismissable || m_activeInput || m_stack.size() <= 1)
		return;

	QWidget *rootPage = m_stack.first();
	m_window->pageStack()->setCurrentWidget(rootPage);
	while (m_stack.size() > 1) {
		QWidget *widget = m_stack.pop();
		m_window->pageStack()->removeWidget(widget);
		widget->deleteLater();
	}
}

InputProviderWidget::Result RootController::presentInput(InputProviderWidget *inputProvider)
{
	if (!inputProvider || !m_window)
		return InputProviderWidget::Rejected;
	if (m_activeInput) {
		qWarning() << "Nested input overlays are not supported";
		return InputProviderWidget::Rejected;
	}

	QWidget *originalParent = inputProvider->parentWidget();
	if (!originalParent)
		originalParent = currentWidget();

	m_activeInput = inputProvider;
	inputProvider->resetResult();
	inputProvider->setParent(m_window->overlayLayer());
	m_window->showOverlay(inputProvider);

	QEventLoop eventLoop;
	const QMetaObject::Connection finishedConnection = connect(
		inputProvider, &InputProviderWidget::finished,
		&eventLoop, &QEventLoop::quit);
	eventLoop.exec();
	disconnect(finishedConnection);

	const InputProviderWidget::Result result = inputProvider->result();
	m_window->hideOverlay(inputProvider);
	inputProvider->setParent(originalParent);
	m_activeInput = nullptr;
	return result;
}

unsigned int RootController::depth() const
{
	return static_cast<unsigned int>(m_stack.size());
}

QWidget *RootController::currentWidget() const
{
	return m_stack.isEmpty() ? nullptr : m_stack.top();
}

void RootController::setDismissable(bool dismissable)
{
	m_dismissable = dismissable;
}

bool RootController::isDismissable() const
{
	return m_dismissable;
}

bool RootController::navigationAllowed(QWidget *newWidget) const
{
	if (!m_window) {
		qWarning() << "RootController has not been initialized";
		if (newWidget)
			newWidget->deleteLater();
		return false;
	}
	if (m_activeInput) {
		qWarning() << "Navigation is disabled while an input overlay is active";
		if (newWidget)
			newWidget->deleteLater();
		return false;
	}
	if (!newWidget) {
		qWarning() << "Cannot navigate to a null widget";
		return false;
	}
	return true;
}
