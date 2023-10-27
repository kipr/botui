#ifndef _CREATE3WIDGET_H_
#define _CREATE3WIDGET_H_

#include "StandardWidget.h"

#include <QRegularExpression>
namespace Ui
{
	class Create3Widget;
}

class Create3Widget : public StandardWidget
{
Q_OBJECT
public:
	Create3Widget(Device *device, QWidget *parent = 0);
	~Create3Widget();

private slots:
	void coeffChanged();
	void motorChanged();
    int createConnect();
	int isConnected();
	void indexChanged( int index);
	void update();


private:
	Ui::Create3Widget *ui;


	double m_setpointVal;
	double m_feedbackVal;
	int m_position_1;
	double m_vel_1;


	void updatePids();
};


#endif
