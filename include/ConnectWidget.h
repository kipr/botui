#ifndef _CONNECTWIDGET_H_
#define _CONNECTWIDGET_H_

#include <QWidget>

namespace Ui
{
	class ConnectWidget;
}

class MenuBar;
class StatusBar;
class Device;

class ConnectWidget : public QWidget
{
Q_OBJECT
public:
	ConnectWidget(Device *device, QWidget *parent = 0);
	~ConnectWidget();
	
public slots:
	void connect();
	void other();
	void refresh();
	
private:
	Ui::ConnectWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
};

#endif
