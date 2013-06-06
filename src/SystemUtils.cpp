#include "SystemUtils.h"

#include <QProcess>
#include <QDebug>

#define WHOAMI_PROCESS "whoami"
#define PASSWD_PROCESS "passwd"
#define NEWLINE_CHAR '\n'

QString SystemUtils::currentUser()
{
	QProcess process;
	process.start(WHOAMI_PROCESS);
	if(!process.waitForFinished(1000)) {
		process.kill();
		return QString();
	}
	return QString(process.readAllStandardOutput()).trimmed();
}

bool SystemUtils::setUserPassword(const QString &password, const QString &user)
{
	QStringList args;
	if(password.isEmpty()) args << "--delete";
	const QString username = user.isEmpty() ? currentUser() : user;
	args << username;
	QProcess process;
	process.start(PASSWD_PROCESS, args);
	if(!process.waitForStarted(1000)) {
		process.kill();
		return false;
	}
	
	process.write(password.toUtf8());
	process.putChar(NEWLINE_CHAR);
	
	// Confirmation
	process.write(password.toUtf8());
	process.putChar(NEWLINE_CHAR);
	
	if(!process.waitForFinished(1000)) {
		process.kill();
		return false;
	}
	
	return process.exitCode() == 0;
}

bool SystemUtils::isUserPasswordSet(const QString &user)
{
	QStringList args;
	args << "-S";
	const QString username = user.isEmpty() ? currentUser() : user;
	args << username;
	QProcess process;
	process.start(PASSWD_PROCESS, args);
	
	if(!process.waitForFinished(1000)) {
		process.kill();
		return false;
	}
	
	if(process.exitCode() != 0) return false;
	
	const QStringList parts = QString(process.readAllStandardOutput()).trimmed().split(" ");
	return parts.size() >= 2 ? parts[1] == "P" : false;
}