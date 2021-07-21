#ifndef _SETTINGSWIDGET_H_
#define _SETTINGSWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
        class AdvancedSettingsWidget;
}

class SettingsWidget : public StandardWidget
{
Q_OBJECT
public:
        AdvancedSettingsWidget(Device *device, QWidget *parent = 0);
        ~AdvancedSettingsWidget();
	
public slots:
	void network();
	void gui();
	void calibrate();
  	void battery();
        void factory();
	
private:
        Ui::AdvancedSettingsWidget *ui;
};


#endif
