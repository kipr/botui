#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include <QString>
#include <QStringList>
#include <QProcess>
#include <QElapsedTimer>

class Program : public QObject
{
Q_OBJECT
public:
	~Program();
	
	bool isRunning();
	
	QProcess *process() const;
	
	static Program *instance();
	
public slots:
	bool start(const QString& path, const QStringList &arguments = QStringList());
	void stop();
	
signals:
	void started();
	void finished(int exitCode, QProcess::ExitStatus exitStatus);
	
private:
	Program();
	Program(const Program& rhs);
	
	QProcess *m_process;
	QElapsedTimer m_time;
};

#endif
