#include "CursorManager.h"
#include <QFileSystemWatcher>

#include "Device.h"

#include <QApplication>
#include <QFileInfo>
#include <QDebug>

CursorManager::CursorManager()
	: m_forceVisible(false),
	m_watcher(new QFileSystemWatcher(this)),
	m_device(0)
{
	m_watcher->addPath("/dev/input/mouse1");
	connect(m_watcher, SIGNAL(fileChanged(QString)),
		SLOT(fileChanged(QString)));
}

void CursorManager::forceCursorVisible(bool forceVisible)
{
	m_forceVisible = forceVisible;
	update();
}

bool CursorManager::isCursorVisible() const
{
	return m_visible;
}

void CursorManager::setDevice(Device *device)
{
	m_device = device;
	update();
}

void CursorManager::fileChanged(const QString &path)
{
	qDebug() << "File changed!!!" << path;
	m_visible = QFileInfo(path).exists();
	qDebug() << m_visible;
	update();
}

void CursorManager::update()
{
	if(m_device && !m_device->isTouchscreen()) return;
	if(m_forceVisible || m_visible) qApp->setOverrideCursor(QCursor(Qt::BlankCursor));
	else qApp->setOverrideCursor(QCursor());
}