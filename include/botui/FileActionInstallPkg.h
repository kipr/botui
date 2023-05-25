#ifndef _FILEACTIONINSTALLPKG_H_
#define _FILEACTIONINSTALLPKG_H_

#include "FileActions.h"

class FileActionInstallPkg : public FileActionExtension {
  public:
    FileActionInstallPkg();
    virtual bool act(const QString &path, Device *device) const;
};

#endif
