#ifndef _MENUBAR_H_
#define _MENUBAR_H_

#include <QMenuBar>
#include <QPixmap>

class QLabel;
class StopButtonWidget;
class Device;
class Program;

class MenuBar : public QMenuBar
{
Q_OBJECT
Q_PROPERTY(QString title READ title WRITE setTitle)
public:
	MenuBar(Program *program, QWidget *parent = 0);
	MenuBar(QWidget *parent = 0);
	~MenuBar();
	
	void setTitle(const QString& title, const QPixmap& icon = QPixmap());
	QString title() const;
	
	void addHomeAndBackButtons();
	
private:
	void init();
	
	QLabel *m_title;
	Program *m_program;
	StopButtonWidget *m_stopButton;
};

#endif
