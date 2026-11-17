#include "RootWindow.h"

#include <QPalette>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QStackedLayout>
#include <QStackedWidget>

RootWindow::RootWindow(QWidget *parent)
	: QWidget(parent),
	  m_pageStack(new QStackedWidget(this)),
	  m_overlayLayer(new QWidget(this)),
	  m_overlayWidget(nullptr)
{
	QPalette contentPalette = palette();
	// Mark the normal window color as explicit so it cannot resolve to the
	// shell's black startup color through parent palette inheritance.
	contentPalette.setColor(QPalette::Window,
	                        contentPalette.color(QPalette::Window));

	setObjectName(QStringLiteral("rootWindow"));
	setAutoFillBackground(true);
	QPalette windowPalette = contentPalette;
	windowPalette.setColor(QPalette::Window, Qt::black);
	setPalette(windowPalette);

	// The black shell prevents an exposed frame during startup, but must not be
	// inherited by pages or input providers.
	m_pageStack->setPalette(contentPalette);
	m_pageStack->setAutoFillBackground(true);
	m_pageStack->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	m_overlayLayer->setObjectName(QStringLiteral("inputOverlayLayer"));
	m_overlayLayer->setAutoFillBackground(true);
	m_overlayLayer->setPalette(contentPalette);
	m_overlayLayer->hide();

	QStackedLayout *layers = new QStackedLayout(this);
	layers->setContentsMargins(0, 0, 0, 0);
	layers->setSizeConstraint(QLayout::SetNoConstraint);
	layers->setStackingMode(QStackedLayout::StackAll);
	layers->addWidget(m_pageStack);
	layers->addWidget(m_overlayLayer);
}

QStackedWidget *RootWindow::pageStack() const
{
	return m_pageStack;
}

QWidget *RootWindow::overlayLayer() const
{
	return m_overlayLayer;
}

void RootWindow::showOverlay(QWidget *widget)
{
	if (!widget)
		return;

	m_overlayWidget = widget;
	widget->setGeometry(m_overlayLayer->rect());
	m_overlayLayer->show();
	m_overlayLayer->raise();
	widget->show();
	widget->raise();
	widget->setFocus(Qt::OtherFocusReason);
}

void RootWindow::hideOverlay(QWidget *widget)
{
	if (widget)
		widget->hide();

	if (!widget || widget == m_overlayWidget) {
		m_overlayWidget = nullptr;
		m_overlayLayer->hide();
	}
}

void RootWindow::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
	if (m_overlayWidget)
		m_overlayWidget->setGeometry(m_overlayLayer->rect());
}
