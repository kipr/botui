#ifndef _SYSTEMUTILS_H_
#define _SYSTEMUTILS_H_

#include <QString>

class SystemUtils {
  public:
    static QString currentUser();
    static bool setUserPassword(const QString &password,
                                const QString &user = QString());
    static bool isUserPasswordSet(const QString &user = QString());
    static QString sha256(const QString &str);
};

#endif
