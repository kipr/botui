#include "KanshiProcess.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <cerrno>
#include <csignal>
#include <cstring>
#include <unistd.h>

uint KanshiProcess::selectUserId(uint effectiveId, const QByteArray &sudoId, uint homeOwnerId)
{
	if (effectiveId != 0)
		return effectiveId;
	bool valid = false;
	const uint invokingId = sudoId.toUInt(&valid);
	if (valid)
		return invokingId;
	return homeOwnerId;
}

uint KanshiProcess::sessionUserId()
{
	return selectUserId(geteuid(), qgetenv("SUDO_UID"),
	                    QFileInfo(QDir::homePath()).ownerId());
}

QVector<qint64> KanshiProcess::find(const QString &procPath, uint userId)
{
	QVector<qint64> processes;
	const QDir proc(procPath);
	for (const QString &entry : proc.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
		bool valid = false;
		const qint64 pid = entry.toLongLong(&valid);
		if (!valid || pid <= 0 || QFileInfo(proc.filePath(entry)).ownerId() != userId)
			continue;
		QFile name(proc.filePath(entry + QStringLiteral("/comm")));
		if (name.open(QIODevice::ReadOnly) && name.readAll().trimmed() == "kanshi")
			processes.append(pid);
	}
	return processes;
}

QVector<qint64> KanshiProcess::findForSession()
{
	return find(QStringLiteral("/proc"), sessionUserId());
}

bool KanshiProcess::reload(QString *errorMessage)
{
	const uint userId = sessionUserId();
	const QVector<qint64> processes = find(QStringLiteral("/proc"), userId);
	if (processes.isEmpty()) {
		if (errorMessage) *errorMessage = QStringLiteral("No Kanshi process owned by session UID %1 was found").arg(userId);
		return false;
	}
	int signaled = 0;
	int failure = 0;
	for (qint64 pid : processes) {
		if (::kill(static_cast<pid_t>(pid), SIGHUP) == 0)
			++signaled;
		else if (errno != ESRCH)
			failure = errno;
	}
	if (signaled == 0 || failure != 0) {
		if (errorMessage) {
			*errorMessage = failure
				? QStringLiteral("Could not reload Kanshi: %1").arg(QString::fromLocal8Bit(std::strerror(failure)))
				: QStringLiteral("Kanshi exited before it could be reloaded");
		}
		return false;
	}
	return true;
}
