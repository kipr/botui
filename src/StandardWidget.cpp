#include "StandardWidget.h"

#include "Device.h"
#include "MenuBar.h"
#include "StatusBar.h"
#include <QLayout>

StandardWidget::StandardWidget(Device *device, QWidget *parent)
    : QWidget(parent),
      m_device(device),
      m_menuBar(new MenuBar(this)),
      m_statusBar(0) {}

StandardWidget::~StandardWidget() {
    delete m_menuBar;
    delete m_statusBar;
}

void StandardWidget::performStandardSetup(const QString &title,
                                          const bool &statusBar) {
    m_menuBar->addHomeAndBackButtons();
    m_menuBar->setTitle(title);
    if (layout()) layout()->setMenuBar(m_menuBar);

    if (!statusBar) return;
    m_statusBar = new StatusBar(this);
    m_statusBar->loadDefaultWidgets(m_device);
    if (layout()) layout()->addWidget(m_statusBar);
}

Device *StandardWidget::device() const { return m_device; }

MenuBar *StandardWidget::menuBar() const { return m_menuBar; }

StatusBar *StandardWidget::statusBar() const { return m_statusBar; }