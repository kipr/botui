#ifndef _FILEMANAGERWIDGET_H_
#define _FILEMANAGERWIDGET_H_

#include <QWidget>
#include <QAction>

namespace Ui
{
	class FileManagerWidget;
}

class QFileSystemModel;
class MenuBar;

class FileManagerWidget : public QWidget
{
Q_OBJECT
public:
	FileManagerWidget(QWidget *parent = 0);
	~FileManagerWidget();
	
private slots:
	void exit();
	void up();
	
	void open();
	void copy();
	void paste();
	void remove();
	void home();
	
private:
	void updateOptions();
	
	Ui::FileManagerWidget *ui;
	MenuBar *m_menuBar;
	QFileSystemModel *m_fs;
	QAction *m_up;
};

#endif
