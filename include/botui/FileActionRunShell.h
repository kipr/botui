#ifndef _FILEACTIONRUNSHELL_H_
#define _FILEACTIONRUNSHELL_H_

#include "FileActions.h"

class FileActionRunShell : public FileActionExtension {
  public:
    FileActionRunShell();
    virtual bool act(const QString &path, Device *device) const;
};

#endif
