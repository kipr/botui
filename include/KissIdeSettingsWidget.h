#ifndef _KISSIDESETTINGSWIDGET_H_
#define _KISSIDESETTINGSWIDGET_H_

#include <QWidget>

namespace Ui
{
	class KissIdeSettingsWidget;
}

class MenuBar;
class StatusBar;
class Device;

class KissIdeSettingsWidget : public QWidget
{
Q_OBJECT
public:
	KissIdeSettingsWidget(Device *device, QWidget *parent = 0);
	~KissIdeSettingsWidget();
	
private:
	Ui::KissIdeSettingsWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
};

#endif
