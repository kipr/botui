#include "DeclarativeView.h"

#include <QFileSystemWatcher>
#include <QQmlEngine>
#include <QtWidgets>

DeclarativeView::DeclarativeView(const QUrl &url,
                                 const bool &autoReload,
                                 QWidget *parent)
    : QQuickWidget(url, parent), m_autoReload(false), m_watcher(0) {
    setAutoReload(autoReload);
}

DeclarativeView::~DeclarativeView() { destroyWatcher(); }

void DeclarativeView::setAutoReload(const bool &autoReload) {
    m_autoReload = autoReload;

    if (m_autoReload)
        createWatcher();
    else
        destroyWatcher();
}

const bool &DeclarativeView::autoReload() const { return m_autoReload; }

void DeclarativeView::reload() {
    engine()->clearComponentCache();
    QUrl url = source();
    setSource(QUrl());
    setSource(url); // Force reload
}

void DeclarativeView::createWatcher() {
    if (source().toLocalFile().isEmpty()) return;
    m_watcher = new QFileSystemWatcher();
    m_watcher->addPath(source().toLocalFile());
    connect(m_watcher, SIGNAL(fileChanged(QString)), SLOT(reload()));
}

void DeclarativeView::destroyWatcher() {
    if (!m_watcher) return;
    m_watcher->disconnect(this);
    delete m_watcher;
}
