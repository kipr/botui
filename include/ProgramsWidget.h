#ifndef _PROGRAMSWIDGET_H_
#define _PROGRAMSWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
	class ProgramsWidget;
}

class ProgramsWidget : public StandardWidget
{
Q_OBJECT
public:
	ProgramsWidget(Device *device, QWidget *parent = 0);
	~ProgramsWidget();
	
public slots:
	void run();
	void args();
	void remove();
	
private:
	Ui::ProgramsWidget *ui;
};

#endif
