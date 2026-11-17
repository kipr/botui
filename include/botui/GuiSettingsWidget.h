#ifndef _GUISETTINGSWIDGET_H_
#define _GUISETTINGSWIDGET_H_

#include "StandardWidget.h"

namespace Ui
{
	class GuiSettingsWidget;
}
class QShowEvent;

class GuiSettingsWidget : public StandardWidget
{
Q_OBJECT
public:
	GuiSettingsWidget(Device *device, QWidget *parent = 0);
	~GuiSettingsWidget();
	
	static void updateStyle(Device *device);
	
private slots:
	void on_invert_screen_clicked();
	
private:
	static void updateWidgets();
	void showEvent(QShowEvent *event) override;
	Ui::GuiSettingsWidget *ui;
};

#endif
