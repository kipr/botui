#ifndef _DECLARATIVEVIEW_H_
#define _DECLARATIVEVIEW_H_

#include <QtQuick/QQuickView>
#include <QtQuickWidgets/QQuickWidget>
/*
 * QDeclarativeView is a Qt4 object that used to be used here.
 * Instead of renaming DeclarativeView (without the Q) to QuickView,
 * only the parent object is renamed to act as a sort of adapter to qt5.
 * Certain parts of code are less confusing this way and it requires less
 * changes
 */

class QFileSystemWatcher;

class DeclarativeView : public QQuickWidget {
    Q_OBJECT
    Q_PROPERTY(bool autoReload READ autoReload WRITE setAutoReload)
  public:
    DeclarativeView(const QUrl &url,
                    const bool &autoReload = false,
                    QWidget *parent = 0);
    ~DeclarativeView();

    void setAutoReload(const bool &autoReload);
    const bool &autoReload() const;
  public slots:
    void reload();

  private:
    void createWatcher();
    void destroyWatcher();

    bool m_autoReload;
    QFileSystemWatcher *m_watcher;
};

#endif
