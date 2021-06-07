#ifndef _PROGRAMSELECTIONWIDGET_H_
#define _PROGRAMSELECTIONWIDGET_H_
#include <QQuickWidget>

namespace Ui
{
	class ProgramSelectionWidget;
}

class MenuBar;
class StatusBar;
class Device;

class ProgramSelectionWidget : public QQuickWidget
{
Q_OBJECT
public:
	ProgramSelectionWidget(Device *device, QQuickWidget *parent = 0);
	~ProgramSelectionWidget();
	
public slots:
	void runningProgram();
	void otherPrograms();
	
private:
	Ui::ProgramSelectionWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
};


#endif
