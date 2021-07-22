#ifndef _FACTORYWIDGET_H_
#define _FACTORYWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
	class FactoryWidget;
}

class FactoryWidget : public StandardWidget
{
Q_OBJECT
public:
	FactoryWidget(Device *device, QWidget *parent = 0);
	~FactoryWidget();
	
public slots:
	void confirm();
        void reflash();
        void experimental();
	
private:
	
	Ui::FactoryWidget *ui;
};

#endif
