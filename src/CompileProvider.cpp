#include "CompileProvider.h"

CompileProvider::CompileProvider(QObject *parent) : QObject(parent) {}

CompileProvider::~CompileProvider() {}

void CompileProvider::setBinariesPath(const QString &binariesPath) {
    m_binariesPath = binariesPath;
}

const QString &CompileProvider::binariesPath() const { return m_binariesPath; }