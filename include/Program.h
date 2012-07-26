#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include <QString>
#include <QProcess>
#include <QTime>

class Program : public QObject
{
Q_OBJECT
public:
	~Program();
	
	bool start(const QString& path);
	bool isRunning();
	
	QProcess *process() const;
	
	static Program *instance();
public slots:
	void stop();
	
signals:
	void started();
	void finished(int exitCode, QProcess::ExitStatus exitStatus);
	
private:
	Program();
	Program(const Program& rhs);
	
	QProcess *m_process;
	QTime m_time;
};

#endif
