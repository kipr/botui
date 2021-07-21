#ifndef _FILEMANAGERWIDGET_H_
#define _FILEMANAGERWIDGET_H_

#include <QQuickWidget>
#include <QAction>
#include <QItemSelection>

namespace Ui
{
	class FileManagerWidget;
}

class QFileSystemModel;
class MenuBar;
class Device;

class FileManagerWidget : public QQuickWidget
{
Q_OBJECT
public:
	FileManagerWidget(Device *device, QQuickWidget *parent = 0);
	~FileManagerWidget();
	
private slots:
	void exit();
	void up();
	
	void open();
	void copy();
	void paste();
	void remove();
	void home();
	
	void selectionChanged(const QItemSelection &selected);
	
private:
	void updateOptions();
	void compile(const QString &path);
	
	Ui::FileManagerWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	QFileSystemModel *m_fs;
	QAction *m_up;
};

#endif
