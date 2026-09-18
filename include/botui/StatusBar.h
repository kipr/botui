#ifndef _STATUSBAR_H_
#define _STATUSBAR_H_

#include <QStatusBar>

class StatusBar : public QStatusBar
{
public:
	StatusBar(QWidget *parent = 0);
	
	void loadDefaultWidgets();
	void addPermanentEventModeLabel();
	void removePermanentEventModeLabel();
private:
	
};


#endif
