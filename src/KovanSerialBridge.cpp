#include "KovanSerialBridge.h"

#include "Program.h"
#include "ProgramWidget.h"
#include "RootController.h"

#include <QDebug>
#include <QLocalServer>
#include <QLocalSocket>

KovanSerialBridge::KovanSerialBridge() : m_device(0) {}

void KovanSerialBridge::init(Device *device) {
    static bool s_inited = false;
    if (s_inited) return;
    m_device = device;
    QLocalServer *socket = new QLocalServer(this);
    connect(socket, SIGNAL(newConnection()), SLOT(newConnection()));
    if (!socket->listen("org.kipr.botui.Run")) {
        qWarning() << "Failed to launch kovan serial bridge";
    }

    s_inited = true;
}

void KovanSerialBridge::newConnection() {
    QLocalSocket *socket =
        qobject_cast<QLocalServer *>(sender())->nextPendingConnection();
    if (!socket) return;
    if (!socket->waitForReadyRead(250)) return;
    run(socket->readAll());
    socket->close();
    delete socket;
}

void KovanSerialBridge::run(const QString &program) {
    ProgramWidget *programWidget =
        new ProgramWidget(Program::instance(), m_device);
    if (!Program::instance()->start(program))
        delete programWidget;
    else
        RootController::ref().presentWidget(programWidget);
}