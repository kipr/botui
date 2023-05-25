#include "ProcessLogDialog.h"

#include <QProcess>

ProcessLogDialog::ProcessLogDialog(QWidget *const parent) : LogDialog(parent) {}

bool ProcessLogDialog::start(const QString &path, const QStringList &args) {
    setDismissable(false);
    _process = new QProcess(this);
    _process->start(path, args);
    _process->setProcessChannelMode(QProcess::MergedChannels);
    connect(_process, SIGNAL(readyReadStandardOutput()), SLOT(readOutput()));
    connect(_process,
            SIGNAL(finished(int, QProcess::ExitStatus)),
            SLOT(terminated()));
    if (!_process->waitForStarted()) {
        delete _process;
        setDismissable(true);
        setStatus(tr("Failed to start!"));
        return false;
    }

    setStatus(tr("Running..."));
    return true;
}

void ProcessLogDialog::readOutput() { appendText(_process->readAll()); }

void ProcessLogDialog::terminated() {
    setStatus(tr("Process terminated"));
    appendText("\n" + tr("Process returned %1").arg(_process->exitCode()));
    setDismissable(true);
}