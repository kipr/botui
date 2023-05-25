#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_

#include <QString>

class FileUtils {
  public:
    static bool copy(const QString &path, const QString &newPath);
    static bool rm(const QString &path);
};

#endif
