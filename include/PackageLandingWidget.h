#ifndef _PACKAGELANDINGWIDGET_H_
#define _PACKAGELANDINGWIDGET_H_

#include <QWidget>

namespace Ui
{
	class PackageLandingWidget;
}

class MenuBar;
class StatusBar;
class Device;

class PackageLandingWidget : public QWidget
{
Q_OBJECT
public:
	PackageLandingWidget(Device *device, QWidget *parent = 0);
	~PackageLandingWidget();
	
private:
	Ui::PackageLandingWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
};

#endif
