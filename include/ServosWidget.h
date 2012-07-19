#ifndef _SERVOSWIDGET_H_
#define _SERVOSWIDGET_H_

#include <QWidget>

namespace Ui
{
	class ServosWidget;
}

class MenuBar;
class StatusBar;
class Device;

class ServosWidget : public QWidget
{
Q_OBJECT
public:
	ServosWidget(Device *device, QWidget *parent = 0);
	~ServosWidget();
	
private:
	Ui::ServosWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
};


#endif
