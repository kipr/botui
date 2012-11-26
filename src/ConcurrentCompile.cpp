#include "ConcurrentCompile.h"

#include "Device.h"
#include "CompileProvider.h"

ConcurrentCompile::ConcurrentCompile(const QString &name, const Kiss::KarPtr &archive, Device *device)
	: m_name(name),
	m_archive(archive),
	m_device(device)
{
}

void ConcurrentCompile::run()
{
	emit compileStarted(m_name, this);
	m_output = m_device->compileProvider()->compile(m_name, m_archive);
	emit compileFinished(m_output, this);
}

void ConcurrentCompile::setUserData(void *data)
{
	m_userData = data;
}

void *ConcurrentCompile::userData() const
{
	return m_userData;
}

bool ConcurrentCompile::isFinished() const
{
	return !m_output.isEmpty();
}

const Compiler::OutputList &ConcurrentCompile::output() const
{
	return m_output;
}