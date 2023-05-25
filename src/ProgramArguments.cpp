#include "ProgramArguments.h"

#define ARGUMENTS_FILE ("arguments.__pass")
/*
void ProgramArguments::setArguments(const kiss::KarPtr &archive, const
QStringList &arguments)
{
    archive->setFile(ARGUMENTS_FILE, arguments.join("\n").toUtf8());
}

QStringList ProgramArguments::arguments(const kiss::KarPtr &archive)
{
    QByteArray data = archive->data(ARGUMENTS_FILE);
    QString str(data);
    return str.split("\n", QString::SkipEmptyParts);
}
*/
