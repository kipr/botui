#ifndef _COMBINEDMOTORWIDGET_H_
#define _COMBINEDMOTORWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
	class CombinedMotorWidget;
}

class CombinedMotorWidget : public StandardWidget
{
Q_OBJECT
public:
	CombinedMotorWidget(Device *device, QWidget *parent = 0);
	~CombinedMotorWidget();
	
private slots:
	void pwmChanged(double pwm);
	void velChanged(double vel);
	
	void tabChanged(int i);
	
	void stop();
	void go();
	
	void forward();
	void backward();

	void update();

private:
	Ui::CombinedMotorWidget *ui;
};

#endif
