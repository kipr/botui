#ifndef _FACTORYWIDGET_H_
#define _FACTORYWIDGET_H_

#include "StandardWidget.h"

#include <QString>
#include <QProcess>
#include <QDir>

namespace Ui
{
        class FactoryWidget;
}

class InputProviderWidget;

class FactoryWidget : public StandardWidget
{
Q_OBJECT
public:
        FactoryWidget(Device *device, QWidget *parent = 0);
        ~FactoryWidget();

public slots:
        void confirm();
        void reflash();
        void experimental();

private:
        Ui::FactoryWidget *ui;
        InputProviderWidget *m_provider;
        QProcess *m_consoleProc;
};


#endif
