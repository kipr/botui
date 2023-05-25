#include "FileActionRunShell.h"
#include "ProcessLogDialog.h"
#include "RootController.h"

#include <QDebug>

FileActionRunShell::FileActionRunShell()
    : FileActionExtension(QObject::tr("Run"),
                          QStringList() << "sh"
                                        << "bash") {}

bool FileActionRunShell::act(const QString &path, Device *device) const {
    ProcessLogDialog dialog;
    if (!dialog.start("/bin/sh", QStringList() << path)) return false;
    RootController::ref().presentDialog(&dialog);
    return true;
}

REGISTER_FILE_ACTION(FileActionRunShell)