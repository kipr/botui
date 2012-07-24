#ifndef _PROGRAMWIDGET_H_
#define _PROGRAMWIDGET_H_

#include <QWidget>
#include <QTime>
#include <QProcess>
#include "ButtonProvider.h"

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
	void lock();
	bool start();
	void stop();
	
private slots:
	void started();
	void finished(int exitCode, QProcess::ExitStatus exitStatus);
	
	void buttonTextChanged(const ButtonProvider::ButtonId& id, const QString& text);
	
private:
	Ui::ProgramWidget *ui;
	QString m_program;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
	
	QTime m_time;
};

#endif
