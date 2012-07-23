#ifndef _PROGRAMWIDGET_H_
#define _PROGRAMWIDGET_H_

#include <QWidget>

namespace Ui
{
	class ProgramWidget;
}

class MenuBar;
class StatusBar;
class Device;

class ProgramWidget : public QWidget
{
Q_OBJECT
public:
	ProgramWidget(const QString& program, Device *device, QWidget *parent = 0);
	~ProgramWidget();
	
public slots:
	void start();
	void stop();
	
private:
	Ui::ProgramWidget *ui;
	QString m_program;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
};

#endif
