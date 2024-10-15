#ifndef _ABOUTWIDGET_H_
#define _ABOUTWIDGET_H_

#include "StandardWidget.h" // Include here if needed for inheritance
#include <QProcess>

namespace Ui
{
	class AboutWidget;
}

class AboutWidget : public StandardWidget
{
	Q_OBJECT

public:
	AboutWidget(Device *device, QWidget *parent = nullptr);
	~AboutWidget();

	bool getEventModeState();
	void setEventModeState(QString newState);
	QString getRaspberryPiType();

signals:
	void eventModeEnabled();
	void eventModeDisabled();

public slots:
	void developerList();
	void eventModeBackground(int checked);

private:
	Ui::AboutWidget *ui;
	QProcess proc;
};

#endif
