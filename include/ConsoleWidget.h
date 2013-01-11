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
	
	void setProcess(QIODevice *process);
	QIODevice *process() const;

private slots:
	void readStandardOut();

	void startBeep();
	void endBeep();
private:
	void printPrompt();
	
	int inputCharCount;
	QTextCursor curCursorLoc;
	QString cmdStr;
	QStringList cmdHistory;
	int histLocation;
	QString tempCmd;
	QIODevice *m_process;
	
	QPalette m_savedPalette;
};

#endif
