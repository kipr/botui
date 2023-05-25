#ifndef _FILEACTIONS_H_
#define _FILEACTIONS_H_

#include "Singleton.h"

#include <QList>
#include <QString>
#include <QStringList>

class Device;

class FileAction {
  public:
    FileAction(const QString &name);
    virtual ~FileAction();

    const QString &name() const;
    virtual bool canHandle(const QString &path) const = 0;

    virtual bool act(const QString &path, Device *device) const = 0;

  private:
    QString m_name;
};

class FileActionExtension : public FileAction {
  public:
    FileActionExtension(const QString &name, const QStringList &extensions);
    bool canHandle(const QString &path) const;

  private:
    QStringList m_extensions;
};

typedef QList<FileAction *> FileActionList;

class FileActions : public Singleton<FileActions> {
  public:
    FileActions();
    ~FileActions();

    void addAction(FileAction *action);

    FileAction *action(const QString &path) const;
    const FileActionList &actions() const;

  private:
    FileActionList m_actions;
};

#define REGISTER_FILE_ACTION(x)                                                \
    __attribute__((constructor)) static void __##x##__registerFileAction() {   \
        FileActions::ref().addAction(new x());                                 \
    }

#endif
