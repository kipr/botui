#include "SystemUtils.h"

#include <QDebug>
#include <QProcess>
#include <iomanip>
#include <openssl/sha.h>
#include <sstream>

#define WHOAMI_PROCESS "whoami"
#define PASSWD_PROCESS "passwd"
#define NEWLINE_CHAR '\n'

QString SystemUtils::currentUser() {
    QProcess process;
    process.start(WHOAMI_PROCESS);
    if (!process.waitForFinished(1000)) {
        process.kill();
        return QString();
    }
    return QString(process.readAllStandardOutput()).trimmed();
}

bool SystemUtils::setUserPassword(const QString &password,
                                  const QString &user) {
    QStringList args;
    if (password.isEmpty()) args << "--delete";
    const QString username = user.isEmpty() ? currentUser() : user;
    args << username;
    QProcess process;
    process.start(PASSWD_PROCESS, args);
    if (!process.waitForStarted(1000)) {
        process.kill();
        return false;
    }

    process.write(password.toUtf8());
    process.putChar(NEWLINE_CHAR);

    // Confirmation
    process.write(password.toUtf8());
    process.putChar(NEWLINE_CHAR);

    if (!process.waitForFinished(1000)) {
        process.kill();
        return false;
    }

    return process.exitCode() == 0;
}

bool SystemUtils::isUserPasswordSet(const QString &user) {
    QStringList args;
    args << "-S";
    const QString username = user.isEmpty() ? currentUser() : user;
    args << username;
    QProcess process;
    process.start(PASSWD_PROCESS, args);

    if (!process.waitForFinished(1000)) {
        process.kill();
        return false;
    }

    if (process.exitCode() != 0) return false;

    const QStringList parts =
        QString(process.readAllStandardOutput()).trimmed().split(" ");
    return parts.size() >= 2 ? parts[1] == "P" : false;
}

QString SystemUtils::sha256(const QString &str) {
    const std::string stdstr = str.toStdString();
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, stdstr.c_str(), stdstr.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    return QString::fromStdString(ss.str());
}