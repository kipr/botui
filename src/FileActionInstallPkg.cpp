#include "FileActionInstallPkg.h"
#include "ProcessLogDialog.h"
#include "RootController.h"

#include <QDebug>

FileActionInstallPkg::FileActionInstallPkg()
    : FileActionExtension(QObject::tr("Install"), QStringList() << "ipk") {}

bool FileActionInstallPkg::act(const QString &path, Device *device) const {
    ProcessLogDialog dialog;
    if (!dialog.start("/usr/bin/opkg", QStringList() << "install" << path))
        return false;
    RootController::ref().presentDialog(&dialog);
    return true;
}

REGISTER_FILE_ACTION(FileActionInstallPkg)