#ifndef _SERVOSWIDGET_H_
#define _SERVOSWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
	class ServosWidget;
}

class MenuBar;
class StatusBar;
class Device;

class ServosWidget : public StandardWidget
{
Q_OBJECT
public:
	ServosWidget(Device *device, QWidget *parent = 0);
	~ServosWidget();
	
private:
	Ui::ServosWidget *ui;
};


#endif
