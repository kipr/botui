#include "ConcurrentCompile.h"

#include "CompileProvider.h"
#include "Device.h"
#include <QDir>

ConcurrentCompile::ConcurrentCompile(const QFileInfo &file,
                                     const kiss::KarPtr &archive,
                                     Device *device)
    : m_file(file), m_archive(archive), m_device(device) {}

void ConcurrentCompile::run() {
    emit compileStarted(m_file.baseName(), this);
    QDir directory = m_file.absoluteDir();
    directory.cdUp();
    m_output = m_device->compileProvider()->compile(
        m_file.baseName(), m_archive, directory.absolutePath());
    emit compileFinished(m_output, this);
}

void ConcurrentCompile::setUserData(void *data) { m_userData = data; }

void *ConcurrentCompile::userData() const { return m_userData; }

bool ConcurrentCompile::isFinished() const { return !m_output.isEmpty(); }

const Compiler::OutputList &ConcurrentCompile::output() const {
    return m_output;
}