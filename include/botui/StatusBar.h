#ifndef _STATUSBAR_H_
#define _STATUSBAR_H_

#include <QStatusBar>
#include <QWidget>

class Device;

class StatusBar : public QStatusBar
{
public:
	StatusBar(QWidget *parent = 0);
	
	void loadDefaultWidgets(Device *device);
private:
	
};


#endif
