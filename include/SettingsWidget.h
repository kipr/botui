#ifndef _SETTINGSWIDGET_H_
#define _SETTINGSWIDGET_H_

#include <QWidget>

namespace Ui
{
	class SettingsWidget;
}

class MenuBar;
class StatusBar;
class Device;

class SettingsWidget : public QWidget
{
Q_OBJECT
public:
	SettingsWidget(Device *device, QWidget *parent = 0);
	~SettingsWidget();
	
public slots:
	void network();
	void kissIde();
	
private:
	Ui::SettingsWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
};


#endif
