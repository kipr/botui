#ifndef _COMPILINGWIDGET_H_
#define _COMPILINGWIDGET_H_

#include <QWidget>

namespace Ui
{
	class CompilingWidget;
}

class MenuBar;
class StatusBar;
class Device;

class CompilingWidget : public QWidget
{
Q_OBJECT
public:
	CompilingWidget(Device *device, QWidget *parent = 0);
	~CompilingWidget();
	
private slots:
	void compileFinished();
	
private:
	Ui::CompilingWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
};

#endif
