#ifndef _CREATE3WIDGET_H_
#define _CREATE3WIDGET_H_

#include "StandardWidget.h"

#include <QRegularExpression>
#include <QStringListModel>
#include <QProcess>

namespace Ui
{
	class Create3Widget;
}

class Create3SensorModel;

class Create3Widget : public StandardWidget
{
	Q_OBJECT
public:
	Create3Widget(Device *device, QWidget *parent = 0);
	~Create3Widget();

private slots:

	QString getIP();
	void toggleChanged();
	void indexChanged(int index);
	void run();
	void rebootBox();

private:
	Ui::Create3Widget *ui;

	Create3SensorModel *_model;
	double m_setpointVal;
	double m_feedbackVal;
	int m_position_1;
	QStringListModel *model; 
	double m_vel_1;
};

#endif
