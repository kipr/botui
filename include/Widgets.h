#ifndef _WIDGETS_H_
#define _WIDGETS_H_

#include <QWidget>

namespace Ui
{
	class Widgets;
}

class MenuBar;

class Widgets : public QWidget
{
Q_OBJECT
public:
	Widgets(QWidget *parent = 0);
	~Widgets();
	
private:
	Ui::Widgets *ui;
	MenuBar *m_menuBar;
};

#endif
