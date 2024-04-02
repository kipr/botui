#ifndef _TELLOWIDGET_H_
#define _TELLOWIDGET_H_

#include "StandardWidget.h"

#include <QRegularExpression>
#include <QProcess>

namespace Ui
{
	class TelloWidget;
}


class TelloWidget : public StandardWidget
{
Q_OBJECT
public:
	TelloWidget(Device *device, QWidget *parent = 0);
	~TelloWidget();

private slots:
    int telloConnect();

private:
	Ui::TelloWidget *ui;


};


#endif
