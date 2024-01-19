#ifndef _DEVELOPERLISTWIDGET_H_
#define _DEVELOPERLISTWIDGET_H_

#include "StandardWidget.h"
#include<QProcess>

namespace Ui
{
    class DeveloperListWidget;
}

class DeveloperListWidget : public StandardWidget
{
Q_OBJECT
    public:
        DeveloperListWidget(Device *device, QWidget *parent = 0);
        ~DeveloperListWidget();

    private:
        Ui::DeveloperListWidget *ui;
        QProcess proc;
};


#endif