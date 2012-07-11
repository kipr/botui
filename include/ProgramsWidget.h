#ifndef _PROGRAMSWIDGET_H_
#define _PROGRAMSWIDGET_H_

#include <QWidget>

namespace Ui
{
	class ProgramsWidget;
}

class MenuBar;
class StatusBar;
class Device;

class ProgramsWidget : public QWidget
{
Q_OBJECT
public:
	ProgramsWidget(Device *device, QWidget *parent = 0);
	~ProgramsWidget();
	
private:
	Ui::ProgramsWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
};

#endif
