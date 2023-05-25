#include "Program.h"
#include "SystemPrefix.h"

#include <QDebug>
#include <QThread>

Program::~Program() { stop(); }

bool Program::start(const QString &program,
                    const QString &workingDirectory,
                    const QStringList &arguments) {
    if (program.isEmpty()) return false;
    stop();
    m_process = new QProcess(this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString libPath = env.value("LD_LIBRARY_PATH");
    env.insert(
        "LD_LIBRARY_PATH",
        (libPath.isEmpty() ? "" : libPath + ":") +
            SystemPrefix::ref().rootManager()->libDirectoryPaths().join(":"));
    m_process->setProcessEnvironment(env);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_process,
            SIGNAL(finished(int, QProcess::ExitStatus)),
            SIGNAL(finished(int, QProcess::ExitStatus)));
    connect(m_process, SIGNAL(readyRead()), SIGNAL(readyRead()));

    if (!workingDirectory.isEmpty())
        m_process->setWorkingDirectory(workingDirectory);

    m_process->start(program, arguments);
    if (!m_process->waitForStarted()) {
        delete m_process;
        m_process = 0;
        return false;
    }
    emit started();
    m_time.restart();
    return true;
}

void Program::stop() {
    if (!m_process) return;
    m_process->terminate();
    if (!m_process->waitForFinished(2000)) m_process->kill();
    // unused: const int msecs = m_time.elapsed();
    //  write(tr("Finished in %1 seconds").arg(msecs / 1000.0).toAscii());
    delete m_process;
    m_process = 0;
}

bool Program::isRunning() {
    return m_process ? m_process->state() != QProcess::NotRunning : false;
}

QProcess *Program::process() const { return m_process; }

Program *Program::instance() {
    static Program s_program;
    return &s_program;
}

Program::Program() : m_process(0) { m_time.start(); }

Program::Program(const Program &) {}
