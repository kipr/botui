#ifndef _CURSORMANAGER_H_
#define _CURSORMANAGER_H_

#include "Singleton.h"
#include <QObject>
#include <QString>

class QFileSystemWatcher;
class Device;

class CursorManager : public QObject, public Singleton<CursorManager> {
    Q_OBJECT
  public:
    CursorManager();

    void forceCursorVisible(bool forceVisible);
    bool isCursorVisible() const;

    void setDevice(Device *device);

  private slots:
    void dirChanged(const QString &path);

  private:
    void update();

    bool m_forceVisible;
    bool m_visible;

    QFileSystemWatcher *m_watcher;
    Device *m_device;
};

#endif
