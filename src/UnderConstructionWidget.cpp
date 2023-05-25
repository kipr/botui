#include "UnderConstructionWidget.h"
#include "Device.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "ui_UnderConstructionWidget.h"
#include <QDebug>

UnderConstructionWidget::UnderConstructionWidget(Device *device,
                                                 QWidget *parent)
    : QWidget(parent),
      ui(new Ui::UnderConstructionWidget),
      m_device(device),
      m_menuBar(new MenuBar(this)),
      m_statusBar(new StatusBar(this)) {
    ui->setupUi(this);
    QAction *homeAction = m_menuBar->addAction("Home");
    connect(homeAction, SIGNAL(triggered()), SLOT(home()));

    if (RootController::ref().depth() > 2) {
        QAction *backAction = m_menuBar->addAction("Back");
        connect(backAction, SIGNAL(triggered()), SLOT(back()));
    }

    m_menuBar->setTitle("Under Construction");
    layout()->setMenuBar(m_menuBar);
    m_statusBar->loadDefaultWidgets(m_device);
    layout()->addWidget(m_statusBar);
}

UnderConstructionWidget::~UnderConstructionWidget() {
    delete ui;
    delete m_menuBar;
}

void UnderConstructionWidget::back() { RootController::ref().dismissWidget(); }

void UnderConstructionWidget::home() {
    RootController::ref().dismissAllWidgets();
}