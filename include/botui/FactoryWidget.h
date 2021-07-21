#ifndef _FACTORYWIDGET_H_
#define _FACTORYWIDGET_H_

#include <QQuickWidget>

namespace Ui
{
	class FactoryWidget;
}

class NumpadDialog;
class Device;

class FactoryWidget : public QQuickWidget
{
Q_OBJECT
public:
	FactoryWidget(Device *device, QQuickWidget *parent = 0);
	~FactoryWidget();
	
public slots:
	void confirm();
        void reflash();
        void experimental();
	
private:
	Device *m_device;
	NumpadDialog *m_serialNumpad;
	
	Ui::FactoryWidget *ui;
};

#endif
