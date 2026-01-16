#include "Program.h"
#include "SystemPrefix.h"
#include <unistd.h>
#include <QDebug>
#include <QThread>
#include <signal.h>

Program::~Program()
{
	stop();
}

bool Program::start(const QString &path, const QStringList &arguments)
{
	if (path.isEmpty())
		return false;
	stop();
	m_process = new QProcess(this);
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	const QString libPath = env.value("LD_LIBRARY_PATH");
	env.insert("LD_LIBRARY_PATH", (libPath.isEmpty() ? "" : libPath + ":") + SystemPrefix::ref().rootManager()->libDirectoryPaths().join(":"));
	m_process->setProcessEnvironment(env);
	m_process->setProcessChannelMode(QProcess::MergedChannels);
	connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SIGNAL(finished(int, QProcess::ExitStatus)));
	connect(m_process, SIGNAL(readyRead()), SIGNAL(readyRead()));

	m_process->start(path, arguments);
	if (!m_process->waitForStarted())
	{
		stop();
		return false;
	}
	emit started();
	m_time.restart();
	return true;
}

void Program::stop()
{
    if (!m_process) return;

    QProcess *p = m_process;
    m_process = nullptr;      // prevent re-entrancy/UAF
    p->disconnect(this);

    p->terminate();
    if (!p->waitForFinished(2000)) {
        p->kill();
        p->waitForFinished(2000); // IMPORTANT: reap after kill
    }

    p->deleteLater();
}

bool Program::isRunning()
{
	return m_process ? m_process->state() != QProcess::NotRunning : false;
}

QProcess *Program::process() const
{
	return m_process;
}

Program *Program::instance()
{
	static Program s_program;
	return &s_program;
}

Program::Program()
		: m_process(0)
{
	m_time.start();
}

Program::Program(const Program &) {}
