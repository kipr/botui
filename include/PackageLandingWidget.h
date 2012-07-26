#ifndef _PACKAGELANDINGWIDGET_H_
#define _PACKAGELANDINGWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
	class PackageLandingWidget;
}

class PackageLandingWidget : public StandardWidget
{
Q_OBJECT
public:
	PackageLandingWidget(Device *device, QWidget *parent = 0);
	~PackageLandingWidget();
	
private:
	Ui::PackageLandingWidget *ui;
};

#endif
