#ifndef _CONSOLEWIDGET_H_
#define _CONSOLEWIDGET_H_

#include <QTextEdit>
#include <QProcess>

class ConsoleWidget : public QTextEdit
{
Q_OBJECT
public:
	ConsoleWidget(QWidget *parent = 0);
	~ConsoleWidget();
	
	void setProcess(QProcess *process);
	QProcess *process() const;
	
protected:
	void keyPressEvent(QKeyEvent * event);
	void changeDir(const QString & dir);

private slots:
	void readStandardOut();
	void readStandardErr();

private:
	void printPrompt();
	
	int inputCharCount;
	QTextCursor curCursorLoc;
	QString cmdStr;
	QStringList cmdHistory;
	int histLocation;
	QString tempCmd;
	QProcess *m_process;
};

#endif
