#ifndef _ABOUTWIDGET_H_
#define _ABOUTWIDGET_H_

#include <QWidget>

namespace Ui
{
	class AboutWidget;
}

class MenuBar;
class StatusBar;
class Device;

class AboutWidget : public QWidget
{
Q_OBJECT
public:
	AboutWidget(Device *device, QWidget *parent = 0);
	~AboutWidget();
	
private:
	Ui::AboutWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
};


#endif