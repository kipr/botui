#include "CursorManager.h"
#include <QFileSystemWatcher>

#include "Device.h"

#include <QApplication>
#include <QDebug>
#include <QFileInfo>

CursorManager::CursorManager()
    : m_forceVisible(true),
      m_watcher(new QFileSystemWatcher(this)),
      m_device(0) {
    m_watcher->addPath("/dev/input");
    connect(m_watcher,
            SIGNAL(directoryChanged(QString)),
            SLOT(dirChanged(QString)));
    update();
}

void CursorManager::forceCursorVisible(bool forceVisible) {
    m_forceVisible = forceVisible;
    update();
}

bool CursorManager::isCursorVisible() const { return m_visible; }

void CursorManager::setDevice(Device *device) {
    m_device = device;
    update();
}

void CursorManager::dirChanged(const QString &path) {
    m_visible = QFileInfo(QString("%1/mouse1").arg(path)).exists();
    update();
}

void CursorManager::update() {
    if (m_device && !m_device->isTouchscreen()) {
        qApp->setOverrideCursor(QCursor());
        return;
    }
    if (m_forceVisible || m_visible)
        qApp->setOverrideCursor(QCursor());
    else
        qApp->setOverrideCursor(QCursor(Qt::BlankCursor));
}
