#ifndef _DECLARATIVEVIEW_H_
#define _DECLARATIVEVIEW_H_

#include <QQuickView>
#include <QQuickWidget>

class QFileSystemWatcher;

class DeclarativeView : public QQuickWidget
{
Q_OBJECT
Q_PROPERTY(bool autoReload READ autoReload WRITE setAutoReload)
public:
	DeclarativeView(const QUrl& url, const bool& autoReload = false, QWidget *parent = 0);
	~DeclarativeView();
	
	void setAutoReload(const bool& autoReload);
	const bool& autoReload() const;
public slots:
	void reload();
	
private:
	void createWatcher();
	void destroyWatcher();
	
	bool m_autoReload;
	QFileSystemWatcher *m_watcher;
};

#endif
