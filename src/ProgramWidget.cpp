#include "ProgramWidget.h"
#include "CompileProvider.h"
#include "Device.h"
#include "LockScreen.h"
#include "MenuBar.h"
#include "Program.h"
#include "RootController.h"
#include "StatusBar.h"
#include "UiStandards.h"
#include "ui_ProgramWidget.h"

#include <QDebug>
#include <QTimer>

ProgramWidget::ProgramWidget(Program *program, Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::ProgramWidget),
      m_program(program),
      m_running(m_program->isRunning()) {
    ui->setupUi(this);
    performStandardSetup(tr("Program"), false);
    // QAction *lock = menuBar()->addAction(UiStandards::lockString());
    // connect(lock, SIGNAL(triggered()), SLOT(lock()));

    ui->extra->setVisible(false);

    // Stop Button
    QAction *m_button = menuBar()->addAction("Stop");
    connect(m_button, SIGNAL(triggered()), SLOT(stoppressed()));

    connect(m_program, SIGNAL(started()), SLOT(started()));
    connect(m_program,
            SIGNAL(finished(int, QProcess::ExitStatus)),
            SLOT(finished(int, QProcess::ExitStatus)));

    ui->console->setProcess(m_program->process());

    ButtonProvider *buttonProvider = device->buttonProvider();
    ui->normal->setEnabled(buttonProvider);
    ui->normal->setVisible(buttonProvider);
    ui->extra->setEnabled(buttonProvider);
    ui->extra->setVisible(buttonProvider);

    if (buttonProvider) {
        ui->extra->setVisible(buttonProvider->isExtraShown());

        ui->a->setText(buttonProvider->text(ButtonProvider::A));
        ui->b->setText(buttonProvider->text(ButtonProvider::B));
        ui->c->setText(buttonProvider->text(ButtonProvider::C));
        ui->x->setText(buttonProvider->text(ButtonProvider::X));
        ui->y->setText(buttonProvider->text(ButtonProvider::Y));
        ui->z->setText(buttonProvider->text(ButtonProvider::Z));

        connect(ui->a, SIGNAL(pressed()), SLOT(aPressed()));
        connect(ui->b, SIGNAL(pressed()), SLOT(bPressed()));
        connect(ui->c, SIGNAL(pressed()), SLOT(cPressed()));
        connect(ui->x, SIGNAL(pressed()), SLOT(xPressed()));
        connect(ui->y, SIGNAL(pressed()), SLOT(yPressed()));
        connect(ui->z, SIGNAL(pressed()), SLOT(zPressed()));

        connect(ui->a, SIGNAL(released()), SLOT(aReleased()));
        connect(ui->b, SIGNAL(released()), SLOT(bReleased()));
        connect(ui->c, SIGNAL(released()), SLOT(cReleased()));
        connect(ui->x, SIGNAL(released()), SLOT(xReleased()));
        connect(ui->y, SIGNAL(released()), SLOT(yReleased()));
        connect(ui->z, SIGNAL(released()), SLOT(zReleased()));

        connect(buttonProvider,
                SIGNAL(buttonTextChanged(ButtonProvider::ButtonId, QString)),
                SLOT(buttonTextChanged(ButtonProvider::ButtonId, QString)));
        connect(buttonProvider,
                SIGNAL(extraShownChanged(bool)),
                SLOT(extraShownChanged(bool)));

        QTimer *timer = new QTimer(this);
        timer->start(100);
        buttonProvider->connect(timer, SIGNAL(timeout()), SLOT(refresh()));

        buttonProvider->reset();
    }
}

void ProgramWidget::stoppressed() {
    ui->console->insertPlainText(QString("\nStop button pressed\n"));
    ui->console->setProcess(0);
    m_program->stop();
    ui->console->insertPlainText(QString("Program exited\n"));
}

void ProgramWidget::lock() { LockScreen::lock(); }

void ProgramWidget::started() {
    ButtonProvider *buttonProvider = device()->buttonProvider();
    if (buttonProvider) buttonProvider->reset();
    ui->console->setProcess(m_program->process());
}

void ProgramWidget::finished(int exitCode, QProcess::ExitStatus exitStatus) {
    ui->console->setProcess(0);
    m_program->stop();
    ui->console->insertPlainText(
        QString("Program exited with code %1\n").arg(exitCode));
}

void ProgramWidget::buttonTextChanged(const ButtonProvider::ButtonId &id,
                                      const QString &text) {
    qDebug() << "Setting button" << id << "to" << text;
    switch (id) {
    case ButtonProvider::A:
        ui->a->setText(text);
        break;
    case ButtonProvider::B:
        ui->b->setText(text);
        break;
    case ButtonProvider::C:
        ui->c->setText(text);
        break;
    case ButtonProvider::X:
        ui->x->setText(text);
        break;
    case ButtonProvider::Y:
        ui->y->setText(text);
        break;
    case ButtonProvider::Z:
        ui->z->setText(text);
        break;
    }
}

void ProgramWidget::extraShownChanged(const bool &shown) {
    ui->extra->setVisible(shown);
    update();

    aReleased();
    bReleased();
    cReleased();
    xReleased();
    yReleased();
    zReleased();
}

void ProgramWidget::aPressed() {
    qDebug() << "SETTING A PRESSED";
    device()->buttonProvider()->setPressed(ButtonProvider::A, true);
}

void ProgramWidget::bPressed() {
    device()->buttonProvider()->setPressed(ButtonProvider::B, true);
}

void ProgramWidget::cPressed() {
    device()->buttonProvider()->setPressed(ButtonProvider::C, true);
}

void ProgramWidget::xPressed() {
    device()->buttonProvider()->setPressed(ButtonProvider::X, true);
}

void ProgramWidget::yPressed() {
    device()->buttonProvider()->setPressed(ButtonProvider::Y, true);
}

void ProgramWidget::zPressed() {
    device()->buttonProvider()->setPressed(ButtonProvider::Z, true);
}

void ProgramWidget::aReleased() {
    device()->buttonProvider()->setPressed(ButtonProvider::A, false);
}

void ProgramWidget::bReleased() {
    device()->buttonProvider()->setPressed(ButtonProvider::B, false);
}

void ProgramWidget::cReleased() {
    device()->buttonProvider()->setPressed(ButtonProvider::C, false);
}

void ProgramWidget::xReleased() {
    device()->buttonProvider()->setPressed(ButtonProvider::X, false);
}

void ProgramWidget::yReleased() {
    device()->buttonProvider()->setPressed(ButtonProvider::Y, false);
}

void ProgramWidget::zReleased() {
    device()->buttonProvider()->setPressed(ButtonProvider::Z, false);
}

ProgramWidget::~ProgramWidget() {
    ui->console->setProcess(0);
    delete ui;
}
