#ifndef _FILEACTIONEDITKAR_H_
#define _FILEACTIONEDITKAR_H_

#include "FileActions.h"

class FileActionEditKar : public FileAction {
  public:
    FileActionEditKar();
    virtual bool canHandle(const QString &path) const;
    virtual bool act(const QString &path, Device *device) const;
};

#endif
