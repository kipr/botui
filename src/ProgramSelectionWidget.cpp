#include "ProgramSelectionWidget.h"
#include "Device.h"
#include "MenuBar.h"
#include "Program.h"
#include "ProgramWidget.h"
#include "ProgramsWidget.h"
#include "RootController.h"
#include "StatusBar.h"
#include "ui_ProgramSelectionWidget.h"
#include <QDebug>

ProgramSelectionWidget::ProgramSelectionWidget(Device *device, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ProgramSelectionWidget),
      m_device(device),
      m_menuBar(new MenuBar(this)),
      m_statusBar(new StatusBar(this)) {
    ui->setupUi(this);
    m_menuBar->addHomeAndBackButtons();
    m_menuBar->setTitle("Program Selection");
    layout()->setMenuBar(m_menuBar);
    m_statusBar->loadDefaultWidgets(m_device);
    layout()->addWidget(m_statusBar);

    connect(ui->runningProgram, SIGNAL(clicked()), SLOT(runningProgram()));
    connect(ui->otherPrograms, SIGNAL(clicked()), SLOT(otherPrograms()));
}

ProgramSelectionWidget::~ProgramSelectionWidget() {
    delete ui;
    delete m_menuBar;
    delete m_statusBar;
}

void ProgramSelectionWidget::runningProgram() {
    RootController::ref().dismissWidget(); // Dismiss ourself
    RootController::ref().presentWidget(
        new ProgramWidget(Program::instance(), m_device));
}

void ProgramSelectionWidget::otherPrograms() {
    RootController::ref().dismissWidget(); // Dismiss ourself
    RootController::ref().presentWidget(new ProgramsWidget(m_device));
}