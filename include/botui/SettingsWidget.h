#ifndef _SETTINGSWIDGET_H_
#define _SETTINGSWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
	class SettingsWidget;
}

class SettingsWidget : public StandardWidget
{
Q_OBJECT
public:
	SettingsWidget(Device *device, QWidget *parent = 0);
	~SettingsWidget();
	
public slots:
	void network();
	void comm();
	void channels();
	void gui();
	void calibrate();
	void language();
  	void update();
  	void hideUi();
  	void battery();
  	void cameraView();
	void backup();
	
private:
	Ui::SettingsWidget *ui;
};


#endif
