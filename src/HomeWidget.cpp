#include "HomeWidget.h"

#include <QApplication>
#include <QMessageBox>
#include <QProcess>

#include "AboutWidget.h"
#include "Device.h"
#include "FileManagerWidget.h"
#include "LockScreen.h"
#include "MenuBar.h"
#include "MotorsSensorsWidget.h"
#include "Program.h"
#include "ProgramSelectionWidget.h"
#include "ProgramsWidget.h"
#include "RootController.h"
#include "SettingsWidget.h"
#include "StatusBar.h"
#include "UiStandards.h"
#include "UnderConstructionWidget.h"
#include "ui_HomeWidget.h"

HomeWidget::HomeWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent), ui(new Ui::HomeWidget) {
    ui->setupUi(this);
    performStandardSetup(UiStandards::homeString());

    connect(ui->programs, SIGNAL(clicked()), SLOT(programs()));
    connect(ui->fileManager, SIGNAL(clicked()), SLOT(fileManager()));
    connect(ui->motorsSensors, SIGNAL(clicked()), SLOT(motorsSensors()));
    connect(ui->settings, SIGNAL(clicked()), SLOT(settings()));

    // TODO: fix fileManager and then remove this line
    ui->fileManager->setVisible(true);

    // QAction *lock = menuBar()->addAction(UiStandards::lockString());
    //  connect(lock, SIGNAL(triggered()), SLOT(lock()));
    QAction *about = menuBar()->addAction(tr("About"));
    QAction *shutDown = menuBar()->addAction(tr("Shut Down"));
    menuBar()->adjustSize();
    connect(about, SIGNAL(triggered()), SLOT(about()));
    connect(shutDown, SIGNAL(triggered()), SLOT(shutDown()));
}

HomeWidget::~HomeWidget() { delete ui; }

void HomeWidget::programs() {
    RootController::ref().presentWidget(
        Program::instance()->isRunning()
            ? (QWidget *)new ProgramSelectionWidget(device())
            : (QWidget *)new ProgramsWidget(device()));
}

void HomeWidget::fileManager() {
    RootController::ref().presentWidget(new FileManagerWidget(device()));
}

void HomeWidget::motorsSensors() {
    RootController::ref().presentWidget(new MotorsSensorsWidget(device()));
}

void HomeWidget::settings() {
    RootController::ref().presentWidget(new SettingsWidget(device()));
}

void HomeWidget::about() {
    RootController::ref().presentWidget(new AboutWidget(device()));
}

void HomeWidget::shutDown() {
#ifdef WOMBAT
    if (QMessageBox::question(this,
                              "Shut Down?",
                              "After system halted, slide power switch off or "
                              "disconnect battery.\n\nContinue?",
                              QMessageBox::Yes | QMessageBox::No) !=
        QMessageBox::Yes)
        return;

    const int ret = QProcess::execute("poweroff");
    if (ret < 0) QMessageBox::information(this, "Failed", "Shut down failed.");
#else
    QMessageBox::information(
        this, "Not Available", "Shut down is only available on the kovan.");
#endif
}

void HomeWidget::lock() { LockScreen::lock(); }
