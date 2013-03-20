#ifndef _CREATEWIDGET_H_
#define _CREATEWIDGET_H_

#include "StandardWidget.h"

class CreateWidget : public StandardWidget
{
	Q_OBJECT
public:
	CreateWidget();
	~CreateWidget();
	
private slots:
	void drive();
	
private:
};

#endif
