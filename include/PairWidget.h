#ifndef _PAIRWIDGET_H_
#define _PAIRWIDGET_H_

#include <QWidget>

namespace Ui
{
	class PairWidget;
}

class MenuBar;
class StatusBar;
class Device;

class PairWidget : public QWidget
{
Q_OBJECT
public:
	PairWidget(Device *device, QWidget *parent = 0);
	~PairWidget();
	
public slots:
	void back();
	
private:
	Ui::PairWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
};

#endif
