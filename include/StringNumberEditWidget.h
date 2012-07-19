#ifndef _STRINGNUMBEREDITWIDGET_H_
#define _STRINGNUMBEREDITWIDGET_H_

#include <QWidget>

namespace Ui
{
	class StringNumberEditWidget;
}

class MenuBar;
class StatusBar;
class Device;
class QNumpadDialog;
class KeyboardDialog;

class StringNumberEditWidget : public QWidget
{
Q_OBJECT
public:
	StringNumberEditWidget(Device *device, QWidget *parent = 0);
	~StringNumberEditWidget();
	
private:
	Ui::StringNumberEditWidget *ui;
	Device *m_device;
	MenuBar *m_menuBar;
	StatusBar *m_statusBar;
	QNumpadDialog *m_numpad;
	KeyboardDialog *m_keyboard;
};

#endif
